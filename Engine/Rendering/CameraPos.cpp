#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_OBJECT_CAMERAPOS
#include "CameraPos.h"
#endif
using namespace Leviathan;
// ------------------------------------ //
#include "Logger.h"

ViewerCameraPos::ViewerCameraPos(){
	FrameTime = 0.0f;
	Pitch = 0.0f;
	Yaw = 0.0f;
	Roll = 0.0f;
	
	pitchup = 0.0f;
	pitchdown = 0.0f;
	yawup = 0.0f;
	yawdown = 0.0f;
	rollup = 0.0f;
	rolldown = 0.0f;

	X = 0;
	Y = 0;

	forward = 0;
	backward = 0;
	left = 0;
	right = 0;

	Z = 0;
	zdown = 0;
	zup = 0;

	TooSmooth = false;

	PitchTarget = 0;
	YawTarget = 0;
	RollTarget = 0;

	XSmooth = 0;
	YSmooth = 0;
	ZSmooth = 0;

	RotSpeed = 2;
	MouseXSensitivity = 1.3f;
	MouseYSensitivity = 1.0f;
}

void ViewerCameraPos::SetFrameTime(float frametime){
	FrameTime = frametime;
}

void ViewerCameraPos::GetRotation(float& yaw, float& pitch, float& roll){
	pitch = Pitch;
	yaw = Yaw;
	roll = Roll;
}

void ViewerCameraPos::GetSmoothRot(float& yaw, float& pitch, float& roll){

	// roll targets toward actual values //
	int tspeedmult = 3;
	// roll values towards target //
	PitchTarget += (Pitch-PitchTarget)/tspeedmult;

	// figure which way to go 

	//int disttozero = Yaw;
	//int disttomax = 360-Yaw;

	//int sectozero = YawTarget;
	//int sectomax = 360-YawTarget;

	int loopfupdowndist = Yaw+(360-YawTarget);
	int loopfdownupdist = (360-Yaw)+YawTarget;
	int directdist = Yaw-YawTarget;
	FORCE_POSITIVE(directdist);

	if((loopfupdowndist < loopfdownupdist) & (loopfupdowndist < directdist)){
		// increase value and loop from 360 to Yaw
		YawTarget += loopfupdowndist/4;

	} else if ((loopfdownupdist < loopfupdowndist) & (loopfdownupdist < directdist)){
		// shortest way is going down and looping to max
		YawTarget -= loopfdownupdist/4;


	} else if ((directdist < loopfdownupdist) & (directdist < loopfupdowndist)){
		// shortest way is directly going towards yaw
		YawTarget += (Yaw-YawTarget)/4;


	} else {
		YawTarget = Yaw;
	}

	if(YawTarget > 360.f){
		float over = YawTarget-360.f;

		YawTarget = 0+over;
	} else if (YawTarget < 0.0f){
		float over = YawTarget;

		YawTarget = 360.f+over;
	}


	//float difference = ((Yaw) - (YawTarget));
	//if(!(difference < 180) & (difference > -180)){
	//	// should go other way //
	//	difference *=-1;
	//}
	//if((difference > 0.4f) | (difference < -0.4f)){
	//	YawTarget += difference/tspeedmult;
	//	if(YawTarget > 360.f){
	//		float over = YawTarget-360.f;

	//		YawTarget = 0+over;
	//	} else if (YawTarget < 0.0f){
	//		float over = YawTarget;

	//		YawTarget = 360.f+over;
	//	}
	//} else {
	//	YawTarget = Yaw;
	//}
	//wstringstream strm;
	//strm << L"VAL datadumb ";
	//strm << Pitch << L" ";
	//strm << Yaw << L" smoothed ";
	//strm << PitchTarget << L" ";
	//strm << YawTarget << L" dirval ";
	//strm << difference;
	//Logger::Get()->Info(strm.str(), false);
	//if((difference < 5) & (difference > 5)){
	//	YawTarget = Yaw;
	//} else {
	//	if(difference < 0){
	//		int tamount = (Yaw-YawTarget)/tspeedmult;
	//		FORCE_POSITIVE(tamount)
	//		YawTarget -= tamount;

	//		if(YawTarget < 0){
	//			YawTarget += 360.f;
	//		} else if(YawTarget > 360.f){
	//			YawTarget -= 360.f;
	//		}


	//	} else if (difference > 0){
	//		int tamount = (Yaw-YawTarget)/tspeedmult;
	//		FORCE_POSITIVE(tamount)
	//		YawTarget += tamount;

	//		if(YawTarget < 0){
	//			YawTarget += 360.f;
	//		} else if(YawTarget > 360.f){
	//			YawTarget -= 360.f;
	//		}


	//	} else {
	//		YawTarget = Yaw;
	//	}
	//}

	pitch = PitchTarget;
	yaw = YawTarget;
	roll = Roll;
}

void ViewerCameraPos::GetPos(float& x, float& y, float& z){
	x = X;
	y = Y;
	z = Z;
}
void ViewerCameraPos::GetSmoothPos(float& x, float& y, float& z){


	XSmooth += (X-XSmooth)/3;
	YSmooth += (Y-YSmooth)/3;
	ZSmooth += (Z-ZSmooth)/3;

	x = XSmooth;
	y = YSmooth;
	z = ZSmooth;
}



void ViewerCameraPos::SetMouseMode(bool mode){
	UseMouse = mode;
}

void ViewerCameraPos::SetSmoothing(bool toset){
	TooSmooth = toset;

}


void ViewerCameraPos::ReadInput(Input* input){
	// this is the main function for moving //
	int xmoved = 0;
	int ymoved = 0;
	input->GetMouseMoveAmount(xmoved, ymoved);

	//xmoved /= 3;
	//ymoved /= 3;

	//if(!((xmoved < 20) & (xmoved > -20))){
	//	xmoved *= 2;
	//}
	//if(!((ymoved < 20) & (ymoved > -20))){
	//	ymoved *= 2;
	//}


	//Yaw += xmoved/6;
	//Pitch += ymoved/6;
	if(TooSmooth){
		int dir = xmoved;
		int ptrun = ymoved;

		if((dir < 4) & (dir > -4)){
			dir = 0;
		}
		if((ptrun < 4) & (ptrun > -4)){
			ptrun = 0;
		}

		PitchTurn(ptrun);
		YawTurn(dir);
	} else {
		if((ymoved/2)*MouseYSensitivity > 180){
			ymoved = 360/MouseYSensitivity;
		}
		if((xmoved/2)*MouseXSensitivity > 360){
			xmoved = 720/MouseXSensitivity;
		}

		// set pitch
		Pitch += (float)ymoved/2 * MouseYSensitivity;
		Yaw += (float)xmoved/2 * MouseXSensitivity;

		if(Pitch > 90){
			Pitch = 90;
		} else if (Pitch < -90)
			Pitch = -90;
		
		while(Yaw > 360){
			float over = Yaw-360.f;

			Yaw = 0+over;

		}
		while (Yaw < 0){
			float under = Yaw;

			Yaw += 360.0f+under;
		}

	}
	//if(Yaw > 360.f){
	//	Yaw = 0;
	//} else if(Yaw < 0.0f){
	//	Yaw = 360.f;
	//}
	//if(Pitch > 360.f){
	//	Pitch = 0;
	//} else if(Pitch < 0.0f){
	//	Pitch = 360.f;
	//}


	if(input->GetKeyPressed((int)'A')){
		SideWays(-1);
	} else if(input->GetKeyPressed((int)'D')){
		SideWays(1);
	} else {
		SideWays(0);
	}

	if(input->GetKeyPressed((int)'W')){
		Forward(1);
	} else if(input->GetKeyPressed((int)'S')){
		Forward(-1);
	} else {
		Forward(0);
	}

	if(input->GetKeyPressed(VK_SPACE)){
		Vertical(1);
	} else if(input->GetKeyPressed(VK_CONTROL)){
		Vertical(-1);
	} else {
		Vertical(0);
	}

	

}

void ViewerCameraPos::PitchTurn(int dir){
	if(dir > 0){
		pitchup += FrameTime * 0.01f;
		pitchdown = 0;
		
		if(pitchup > (FrameTime * 0.10f)){
		
			pitchup = FrameTime * 0.10f;
		}
	
	} else if(dir == 0){
		// slow down //
		pitchup -= FrameTime * 0.01f;
		if(pitchup < 0.0f)
			pitchup = 0.0f;
			
		pitchdown -= FrameTime * 0.01f;
		if(pitchdown < 0.0f)
			pitchdown = 0.0f;
	
	
	} else {
		// dir < 0
		pitchdown += FrameTime * 0.03f;
		pitchup = 0;
		
		if(pitchdown > (FrameTime * 0.10f)){
		
			pitchdown = FrameTime * 0.10f;
		}	
	}
	
	Pitch += pitchup;
	Pitch -= pitchdown;
	if(Pitch < -90.0f){
		Pitch = -90;
	} else if(Pitch > 90.0f)
		Pitch = 90.0f;
}

void ViewerCameraPos::YawTurn(int dir){
	if(dir > 0){
		yawup += FrameTime * 0.03f;
		yawdown = 0;

		if(yawup > (FrameTime * 0.10f)){
		
			yawup = FrameTime * 0.10f;
		}
	
	} else if(dir == 0){
		// slow down //
		yawup -= FrameTime * 0.01f;
		if(yawup < 0.0f)
			yawup = 0.0f;
			
		yawdown -= FrameTime * 0.01f;
		if(yawdown < 0.0f)
			yawdown = 0.0f;
	
	
	} else {
		// dir < 0
		yawdown += FrameTime * 0.03f;
		yawup = 0;
		
		if(yawdown > (FrameTime * 0.10f)){
		
			yawdown = FrameTime * 0.10f;
		}	
	}
	
	Yaw += yawup;
	Yaw -= yawdown;
	if(Yaw < 0.0f){
		Yaw = 360.f;
	} else if(Yaw > 360.f)
		Yaw = 0.0f;
}

void ViewerCameraPos::RollTurn(int dir){
	if(dir > 0){
		rollup += FrameTime * 0.03f;
		rolldown = 0;

		if(rollup > (FrameTime * 0.10f)){
		
			rollup = FrameTime * 0.10f;
		}
	
	} else if(dir == 0){
		// slow down //
		rollup -= FrameTime * 0.010f;
		if(rollup < 0.0f)
			rollup = 0.0f;
			
		rolldown -= FrameTime * 0.010f;
		if(rolldown < 0.0f)
			rolldown = 0.0f;
	
	
	} else {
		// dir < 0
		rolldown += FrameTime * 0.03f;
		rollup = 0;
		
		if(rolldown > (FrameTime * 0.10f)){
		
			rolldown = FrameTime * 0.10f;
		}	
	}
	
	Roll += rollup;
	Roll -= rolldown;
	if(Roll < 0.0f){
		Roll = 360.f;
	} else if(Roll > 360.f)
		Roll = 0.0f;
}

void ViewerCameraPos::SideWays(int dir){
	if(dir > 0){
		right += FrameTime * 0.01f;
		left = 0;

		if(right > (FrameTime * 0.08f)){
		
			right = FrameTime * 0.08f;
		}
	
	} else if(dir == 0){
		// slow down //
		right -= FrameTime * 0.012f;
		if(right < 0.0f)
			right = 0.0f;
			
		left -= FrameTime * 0.012f;
		if(left < 0.0f)
			left = 0.0f;
	
	
	} else {
		// dir < 0
		left += FrameTime * 0.01f;
		right = 0;

		if(left > (FrameTime * 0.08f)){
		
			left = FrameTime * 0.08f;
		}	
	}
	
	// calcs here //
	
	//X +=
	if(right){
		Y += sin(Convert::DegreesToRadians(Yaw+90))*right;
		X += cos(Convert::DegreesToRadians(Yaw+90))*right;
	} else if(left){
		//Y -= sin(Yaw)*cos(Pitch)*left;
		Y += sin(Convert::DegreesToRadians(Yaw-90))*left;
		X += cos(Convert::DegreesToRadians(Yaw-90))*left;
	}


}
void ViewerCameraPos::Forward(int dir){
	if(dir > 0){
		forward += FrameTime * 0.01f;
		backward = 0;

		if(forward > (FrameTime * 0.10f)){
		
			forward = FrameTime * 0.10f;
		}
	
	} else if(dir == 0){
		// slow down //
		forward -= FrameTime * 0.02f;
		if(forward < 0.0f)
			forward = 0.0f;
			
		backward -= FrameTime * 0.02f;
		if(backward < 0.0f)
			backward = 0.0f;
	
	
	} else {
		// dir < 0
		backward += FrameTime * 0.01f;
		forward = 0;
		
		if(backward > (FrameTime * 0.10f)){
		
			backward = FrameTime * 0.10f;
		}	
	}

	if(forward){
		Y += sin(Convert::DegreesToRadians(Yaw))*forward;
		X += cos(Convert::DegreesToRadians(Yaw))*forward;
		Z -= sin(Convert::DegreesToRadians(Pitch))*forward;
	} else if(backward){
		Y -= sin(Convert::DegreesToRadians(Yaw))*backward;
		X -= cos(Convert::DegreesToRadians(Yaw))*backward;
		Z += sin(Convert::DegreesToRadians(Pitch))*backward;
	}	
	
}
void ViewerCameraPos::Vertical(int dir){
	if(dir > 0){
		zup += FrameTime * 0.01f;
		zdown = 0;

		if(zup > (FrameTime * 0.09f)){
		
			zup = FrameTime * 0.09f;
		}
	
	} else if(dir == 0){
		// slow down //
		zup -= FrameTime * 0.01f;
		if(zup < 0.0f)
			zup = 0.0f;
			
		zdown -= FrameTime * 0.01f;
		if(zdown < 0.0f)
			zdown = 0.0f;
	
	
	} else {
		// dir < 0
		zdown += FrameTime * 0.01f;
		zup = 0;
		
		if(zdown > (FrameTime * 0.09f)){
		
			zdown = FrameTime * 0.09f;
		}	
	}
	
	Z += zup;
	Z -= zdown;
}
// ------------------------------------ //
void ViewerCameraPos::SetPos(float x, float y, float z, bool smoothly){
	X = x;
	Y = y;
	Z = z;

	if(!smoothly){
		XSmooth = x;
		YSmooth = y;
		ZSmooth = z;
	}
}
void ViewerCameraPos::SetRotation(float yaw, float pitch, float roll, bool smoothly){
	Yaw = yaw;
	Pitch = pitch;
	Roll = roll;

	if(!smoothly){
		YawTarget = yaw;
		PitchTarget = pitch;
		RollTarget = roll;
	}
}
// ------------------------------------ //

// ------------------------------------ //