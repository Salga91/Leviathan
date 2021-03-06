// ------------------------------------ //
#include "Systems.h"

#include "GameWorld.h"
#include "Networking/Connection.h"
#include "Networking/NetworkHandler.h"
#include "Networking/NetworkRequest.h"
#include "Networking/NetworkResponse.h"
#include "Networking/SentNetworkThing.h"

#include "Animation/OgreSkeletonAnimation.h"
#include "Animation/OgreSkeletonInstance.h"
#include "OgreItem.h"
using namespace Leviathan;
// ------------------------------------ //
// SendableSystem
//! \brief Helper for SendableSystem::HandleNode to not have as much code duplication for
//! client and server code
void SendableHandleHelper(ObjectID id, Sendable& obj, GameWorld& world,
    const std::shared_ptr<Connection>& connection,
    const std::shared_ptr<EntityState>& curstate, bool server)
{
    const auto ticknumber = world.GetTickNumber();

    // Determine if this is initial data or an update
    bool initial = true;

    for(auto iter = obj.UpdateReceivers.begin(); iter != obj.UpdateReceivers.end(); ++iter) {

        if(iter->CorrespondingConnection == connection) {

            // Updating an existing connection
            initial = false;

            // Prepare the update packet data  //
            sf::Packet updateData;

            // Do not use last confirmed if it is too old
            int referencetick = -1;

            if(iter->LastConfirmedData &&
                (ticknumber <
                    iter->LastConfirmedTickNumber + BASESENDABLE_STORED_RECEIVED_STATES - 1)) {
                referencetick = iter->LastConfirmedTickNumber;

                // Now calculate a delta update from curstate to the last confirmed state
                curstate->CreateUpdatePacket(*iter->LastConfirmedData, updateData);

            } else {

                // Data is too old (or doesn't exist) and cannot be used //
                curstate->AddDataToPacket(updateData);

                iter->LastConfirmedTickNumber = -1;
                iter->LastConfirmedData.reset();
            }

            // Send the update packet
            auto sentThing = connection->SendPacketToConnectionWithTrackingWithoutGuarantee(
                ResponseEntityUpdate(
                    0, world.GetID(), ticknumber, referencetick, id, std::move(updateData)));

            iter->AddSentPacket(ticknumber, curstate, sentThing);

            break;
        }
    }

    if(initial) {

        // Only server sends the static state, as the server doesn't want to receive that data,
        // because it was the one who initially sent it to any client that has local control
        if(server) {
            // First create a packet which will store the component initial data and lists all
            // the components
            // TODO: this data could be cached when an entity is created as it will be sent to
            // all the players
            sf::Packet initialComponentData;
            uint32_t componentCount = world.CaptureEntityStaticState(id, initialComponentData);

            // Send the initial response
            connection->SendPacketToConnection(
                std::make_shared<ResponseEntityCreation>(
                    0, world.GetID(), id, componentCount, std::move(initialComponentData)),
                RECEIVE_GUARANTEE::Critical);
        }

        // And then send the initial state packet
        sf::Packet updateData;

        curstate->AddDataToPacket(updateData);

        auto sentThing = connection->SendPacketToConnectionWithTrackingWithoutGuarantee(
            ResponseEntityUpdate(0, world.GetID(), ticknumber, -1, id, std::move(updateData)));

        // And add the connection to the receivers
        obj.UpdateReceivers.emplace_back(connection);
        obj.UpdateReceivers.back().AddSentPacket(ticknumber, curstate, sentThing);
    }
}

DLLEXPORT void SendableSystem::HandleNode(ObjectID id, Sendable& obj, GameWorld& world)
{
    const bool isServer = world.GetNetworkSettings().IsAuthoritative;

    const auto& players = world.GetConnectedPlayers();
    const auto& serverConnection = world.GetServerForLocalControl();

    // Create current state here as one or more connections should require it //
    // TODO: this could be removed entirely if we don't allow delta compression within a single
    // component. Other possible approach is to recycle old captured state objects
    auto curState = std::make_shared<EntityState>();

    world.CaptureEntityState(id, *curState);

    if(!curState) {

        LOG_ERROR(
            "SendableSystem: created invalid state for entity, id: " + Convert::ToString(id));
        return;
    }

    // Detect successful packets and closed connections
    for(auto iter = obj.UpdateReceivers.begin(); iter != obj.UpdateReceivers.end();) {
        // This is closed if the connection is invalid
        if(!iter->CorrespondingConnection->IsValidForSend()) {
            iter = obj.UpdateReceivers.erase(iter);
            continue;
        }

        bool exists = false;

        if(isServer) {
            // Or if the player has been removed from the world

            for(const auto& player : players) {

                if(iter->CorrespondingConnection == player->GetConnection()) {
                    exists = true;
                    break;
                }
            }
        } else {

            if(iter->CorrespondingConnection == serverConnection)
                exists = true;
        }

        if(!exists) {
            iter = obj.UpdateReceivers.erase(iter);
            continue;
        }

        // Check the most recent received state
        iter->CheckReceivedPackets();

        // Still valid, move to next
        ++iter;
    }

    // Handle sending updates
    if(isServer) {
        for(const auto& player : players) {

            const auto& connection = player->GetConnection();

            SendableHandleHelper(id, obj, world, connection, curState, isServer);
        }
    } else {

        if(serverConnection && serverConnection->IsValidForSend()) {

            SendableHandleHelper(id, obj, world, serverConnection, curState, isServer);
        }
    }
}
// ------------------------------------ //
// DLLEXPORT void ReceivedSystem::Run(
//     GameWorld& world, std::unordered_map<ObjectID, Received*>& Index)
// {
//     const float progress = world.GetTickProgress();
//     const auto tick = world.GetTickNumber();

//     for(auto iter = Index.begin(); iter != Index.end(); ++iter) {

//         auto& node = *iter->second;

//         // Unmarked nodes should have invalid interpolation status
//         if(!node.Marked)
//             return;

//         if(!node.LocallyControlled) {

//             // Interpolate received states //
//             float adjustedprogress = progress;
//             const Received::StoredState* first;
//             const Received::StoredState* second;

//             try {
//                 node.GetServerSentStates(&first, &second, tick, adjustedprogress);
//             } catch(const InvalidState&) {

//                 // If not found unmark to avoid running unneeded //
//                 node.Marked = false;
//                 continue;
//             }

//             DEBUG_BREAK;
//             // first->Interpolate(second, adjustedprogress);

//         } else {

//             // Send updates to the server //
//         }
//     }
// }
// ------------------------------------ //
// AnimationTimeAdder
DLLEXPORT void AnimationTimeAdder::Run(
    GameWorld& world, std::unordered_map<ObjectID, Animated*>& index, int tick, int timeintick)
{
    float timeNow = (tick * TICKSPEED + timeintick) / 1000.f;

    const float passed = timeNow - LastSeconds;

    LastSeconds = timeNow;

    for(auto iter = index.begin(); iter != index.end(); ++iter) {

        Animated& animated = *iter->second;

        if(!animated.GraphicalObject)
            continue;

        auto skeleton = animated.GraphicalObject->getSkeletonInstance();

        if(!skeleton) {

            LOG_WARNING("Animated (entity id: " + std::to_string(iter->first) +
                        ") has an Item that has no skeleton instance");
            continue;
        }

        if(animated.Marked) {
            animated.Marked = false;

            // Apply all properties and stop not playing animations
            for(const auto& animation : animated.Animations) {

                try {

                    // Documentation says that this throws if not found
                    Ogre::SkeletonAnimation* ogreAnim = skeleton->getAnimation(animation.Name);

                    ogreAnim->setEnabled(true);
                    ogreAnim->setLoop(animation.Loop);

                } catch(const Ogre::Exception& e) {

                    LOG_WARNING("Animated (entity id: " + std::to_string(iter->first) +
                                ") has no animation named: " + animation.ReadableName +
                                ", exception: " + e.what());
                }
            }

            // Then disable
            for(const auto& ogreAnimation : skeleton->getAnimations()) {

                bool found = false;

                for(const auto& animation : animated.Animations) {

                    if(animation.Name == ogreAnimation.getName()) {
                        found = true;
                        break;
                    }
                }

                if(!found) {

                    skeleton->getAnimation(ogreAnimation.getName())->setEnabled(false);
                }
            }
        }

        // Update animation time
        for(const auto& animation : animated.Animations) {

            if(animation.Paused)
                return;

            try {

                // Documentation says that this throws if not found
                Ogre::SkeletonAnimation* ogreAnim = skeleton->getAnimation(animation.Name);
                ogreAnim->addTime(passed * animation.SpeedFactor);

            } catch(const Ogre::Exception& e) {

                LOG_WARNING("Animated (entity id: " + std::to_string(iter->first) +
                            ") has no animation named (in update): " + animation.ReadableName +
                            ", exception: " + e.what());
            }
        }
    }
}
