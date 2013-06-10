#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_ENGINE
#include "Engine.h"
#endif
using namespace Leviathan;
// ------------------------------------ //
#include "Application.h"

Engine::Engine(){

	Mainlog = NULL;
	Inited = false;
	Wind = NULL;
	Graph = NULL;
	Define = NULL;
	MTimer = NULL;
	MainRandom = NULL;
	RenderTimer = NULL;

	Inputs = NULL;
	Sound = NULL;

	TimePassed = 0;
	LastFrame = 0;

	//FpsMonitor = NULL;
	//CpuUsage = NULL;

	Gui = NULL;
	Mainstore = NULL;
	MainScript = NULL;

	TickCount = 0;
	TickTime = 0;
	//FrameTime = 0;
	FrameCount = 0;

	//RenderStart = 0;
	//SinceRender = 0;

	MainCamera = NULL;
	KeyListener = NULL;
	MainEvents = NULL;
	GObjects = NULL;
	Loader = NULL;
	AdvancedGeometryFiles = NULL;
	OutOMemory = NULL;
	AnimManager = NULL;

	Focused = true;
	MouseCaptured = false;
	WantsToCapture = false;

}
// ------------------------------------ //
bool Engine::InitEngine(Window* wind, bool windowed, AppDef* def){
	// get time, for monitoring how long load takes //
	__int64 InitStartTime = Misc::GetTimeMs64();

	instance = this;

	Define = def;
	// create logger object //
	if(Logger::GetIfExists() != NULL){
		// already exists //
		Mainlog = Logger::Get();
	} else {
		Mainlog = new Logger();
	}

	// create //
	OutOMemory = new OutOfMemoryHandler();


	Wind = wind;

	// create randomizer //
	MainRandom = new Random((int)InitStartTime);
	MainRandom->SetAsMain();

	// update resolution scaling //
	ResolutionScaling::SetResolution(Wind->GetWidth(), Wind->GetHeight());


	// data storage //
	Mainstore = new DataStore(true);
	CLASS_ALLOC_CHECK(Mainstore);
	// set height/width values //
	Mainstore->SetHeight(wind->GetHeight());
	Mainstore->SetWidth(wind->GetWidth());

	// search data folder for files // 
	FileSystem::SearchFiles();



	// object holder //
	GObjects = new ObjectManager();
	if(!GObjects){

		Logger::Get()->Error(L"Engine: 008");
		return false;
	}
	if(!GObjects->Init()){
		Logger::Get()->Error(L"Failed to init Engine, Init ObjectManager failed!");
		return false;
	}



	// create script interface before renderer //
	MainScript = new ScriptInterface();
	if(!MainScript){

		Logger::Get()->Error(L"Engine: 008");
		return false;
	}
	if(!MainScript->Init()){
		Logger::Get()->Error(L"Failed to init Engine, Init ScriptInterface failed!");
		return false;
	}

	def->GetValues()->GetValue(L"MaxFPS", FrameLimit);


	Graph = new Graphics();
	MTimer = new Timer();
	MainEvents = new EventHandler();
	if(!MainEvents){
		Logger::Get()->Error(L"Engine: 008");
		return false;
	}
	if(!MainEvents->Init()){
		Logger::Get()->Error(L"Failed to init Engine, Init EventHandler failed!");
		return false;
	}
	// init graphics //
	D3D_FEATURE_LEVEL flevel = D3D_FEATURE_LEVEL_11_0;
	int targetlevel = 11;
	def->GetValues()->GetValue(L"FeatureLevel",targetlevel);
	switch(targetlevel){
	case 11: flevel = D3D_FEATURE_LEVEL_11_0; break;


	default: break;
	}
	
	// renderer //
	// get values from config //
	int vsyncon = true;
	int dhardware = true;
	D3D_DRIVER_TYPE dtype = D3D_DRIVER_TYPE_HARDWARE;

	LeviathanApplication::GetAppDef()->GetValues()->GetValue(L"Vsync", vsyncon);
	LeviathanApplication::GetAppDef()->GetValues()->GetValue(L"DriverHardWare", dhardware);
	if(dhardware == 0){
		Logger::Get()->Info(L"Driver type is NOT set to hardware, performance warning", true);
		dtype = D3D_DRIVER_TYPE_SOFTWARE;
	}

	int MSAA = 4;
	def->GetValues()->GetValue(L"MSAA",MSAA);

	if(MSAA < 1)
		MSAA = 1;
	if(MSAA > 32)
		MSAA = 32;

	dxconf = DxRendConf(windowed, vsyncon != 0, 1000.0f, 0.1f,dtype, MSAA);
	Graph->SetDescObjects(dxconf);
	// call init //
	if(!Graph->Init(Wind)){
		Logger::Get()->Error(L"Failed to init Engine, Init graphics failed! Aborting");
		return false;
	}
	//// init render variable // // don't init this here or first render time will be off the charts! //
	//RenderStart = Misc::GetTimeMs64();



	// create camera that always exists //
	MainCamera = new ViewerCameraPos();
	MainCamera->SetMouseMode(true);
	MainCamera->SetPos(-10,0,0);
	MainCamera->SetSmoothing(false);

	// file parsing //
	ObjectFileProcessor::Initialize();

	// 3d model animations //
	AnimManager = new AnimationManager();
	if(!AnimManager->Init()){
		Logger::Get()->Error(L"Failed to init Engine, AnimationManager init failed");
		return false;
	}

	// key listening //
	KeyListener = new KeyPressManager();
	if(!KeyListener){
		Logger::Get()->Error(L"Engine: 008");
		return false;
	}

	// Gui //
	Gui = new GuiManager();
	if(!Gui){

		Logger::Get()->Error(L"Engine: 008");
		return false;
	}

	if(!Gui->Init(def)){

		Logger::Get()->Error(L"Failed to init Engine, Gui init failed");
		return false;
	}

	// sound device //
	Sound = new SoundDevice();
	if(!Sound){

		Logger::Get()->Error(L"Engine: 008");
		return false;
	}

	// init //
	if(!Sound->Init()){

		Logger::Get()->Error(L"Failed to init Engine, sound init failed");
		return false;
	}
	
	Inputs = new Input();
	if(!Inputs){
		
		Logger::Get()->Error(L"Engine: 008");
		return false;
	}
	// load control structures //
	if(!Inputs->Init(LeviathanApplication::GetApp()->GetHinstance(), Wind->GetWidth(), Wind->GetHeight())){

		Logger::Get()->Error(L"Failed to init Engine, input init failed");
		return false;
	}

	// start monitors //
	// CPumonitor
	//CpuUsage = new CpuMonitor();
	//if(!CpuUsage){

	//	Logger::Get()->Error(L"Engine: 008");
	//	return false;
	//}
	//// init //
	//if(!CpuUsage->Init()){

	//	Logger::Get()->Error(L"Engine: failed to init Cpumonitor");
	//	return false;
	//}
	// fps counter //
	//FpsMonitor = new FpsCounter();
	//if(!FpsMonitor){

	//	Logger::Get()->Error(L"Engine: 008");
	//	return false;
	//}

	//if(!FpsMonitor->Init()){

	//	// cannot return false

	//}
	// create object loader //
	Loader = new ObjectLoader(this);
	if(!Loader){
		Logger::Get()->Error(L"Engine: 008");
		return false;
	}

	// temporary move

	AdvancedGeometryFiles = new GeometryAdvancedLoader();
	if(!AdvancedGeometryFiles){

		Logger::Get()->Error(L"Engine: 008");
		return false;
	}

	RenderTimer = new RenderingStatistics();
	if(!RenderTimer){
		Logger::Get()->Error(L"Engine: 008");
		return false;
	}



	Inited = true;

	PostLoad();



	int TimeTaken = (int)(Misc::GetTimeMs64()-InitStartTime);
	Logger::Get()->Info(L"Engine init took "+Convert::IntToWstring(TimeTaken)+L" ms");

	// let's send a debug message telling engine initialized //
	DEBUG_OUTPUT(wstring(L"[INFO] Engine initialized\n")); // important new line in the end //
	
	////DEBUG_OUTPUT_AUTO(Engine initialized);
	//wstring model = L"Cube";
	//wstring exts = L"dae";
	//AdvancedGeometryFiles->ScanGeometryFile(FileSystem::SearchForFile(FILEGROUP_MODEL, model, exts));




	return true;
}
bool Engine::ShutDownEngine(){

	// Gui is very picky about delete order
	SAFE_RELEASEDEL(Gui);

	SAFE_DELETE(MainCamera);

	SAFE_RELEASEDEL(MainScript);

	Mainlog->Save();


	SAFE_DELETE(Mainstore);
	SAFE_DELETE(Loader);
	SAFE_DELETE(AdvancedGeometryFiles);
	// file parsing //
	ObjectFileProcessor::Release();

	SAFE_RELEASEDEL(AnimManager);

	//Graph->Release();
	SAFE_RELEASEDEL(Graph);

	SAFE_DELETE(MTimer);

	SAFE_RELEASEDEL(Inputs);
	SAFE_RELEASEDEL(Sound);

	SAFE_DELETE(RenderTimer);

	//SAFE_RELEASEDEL(CpuUsage);
	//SAFE_DELETE(FpsMonitor);
	SAFE_DELETE(KeyListener);

	SAFE_RELEASEDEL(GObjects);

	// delete randomizer last, for obvious reasons //
	SAFE_DELETE(MainRandom);

	FileSystem::ClearFoundFiles();

	Wind = NULL;
	SAFE_DELETE(Mainlog);
	this->Inited = false;

	// safe to delete this here //
	SAFE_DELETE(OutOMemory);

	return true;

}
void Engine::PostLoad(){

	// process models to valid formats //
	AdvancedGeometryFiles->ProcessAllModels();

	// get time //
	LastFrame = Misc::GetTimeMs64();
	//RenderStart = Misc::GetTimeMs64();

	SetGuiActive(true);
	//CaptureMouse(true);
	//vector<wstring> filesingamedirs;
	//FileSystem::GetFilesInDirectory(filesingamedirs, L".\\");


	// increase start count //
	if(!Mainstore->AddValueIfDoesntExist(L"StartCount", 1)){
		// increase //
		Mainstore->SetValue(L"StartCount", Mainstore->GetValue(L"StartCount")+1);
	} else {
		// set as persistent //
		Mainstore->SetPersistance(L"StartCount", true);
	}
}
// ------------------------------------ //
void Engine::Tick(bool Force){
	// get time since last update //
	__int64 CurTime = Misc::GetTimeMs64();
	TimePassed = (int)(CurTime-LastFrame);

	if((TimePassed < TICKSPEED) && (!Force)){
		// no tick time yet //
		return;
	}

	LastFrame = CurTime;
	TickCount++;
	
	// update input //
	Inputs->Update();

	// sound tick //
	Sound->Tick(TimePassed);

	if(this->Focused)
		KeyListener->ProcessInput(Inputs);

	Gui->GuiTick(TimePassed);

	if(Focused){
		if(!GuiActive){
			MainCamera->ReadInput(Inputs, true, true);
		} else if(!Gui->HasForeGround()){
			MainCamera->ReadInput(Inputs, false, true);
		} else {
			MainCamera->ClearInputs();
		}
	} else {
		MainCamera->ClearInputs();
	}

	// update texture usage times, to allow unused textures to be unloaded //
	Graph->GetTextureManager()->TimePass(TimePassed);


	// some dark magic here //
	if(TickCount % 25 == 0){
		//// print some random numbers //
		//for(int times = 0; times < 25; times++){
		//	float number = Random::Get()->GetNumber(0.f, 10.f);
		//	Logger::Get()->Info(L"Random: "+Convert::FloatToWstring(number), false);
		//}

		// update values
		Mainstore->SetTickCount(TickCount);
		Mainstore->SetTickTime(TickTime);
		//Mainstore->SetFrameTime(FrameTime);
		//Mainstore->SetFPS(FpsMonitor->GetFps());


		//// window resize test //
		//int newwidth = Mainstore->GetWidth()+3;
		//int newheight = Mainstore->GetHeight()+3;

		//this->DoWindowResize(newwidth, newheight);
		
		// send updated rendering statistics //
		RenderTimer->ReportStats(Mainstore);
	}

	//TimePassed = -TICKSPEED;
	//TimePassed = 0;
	TickTime = (int)(Misc::GetTimeMs64()-LastFrame);

	// send tick event //
	MainEvents->CallEvent(new Event(EVENT_TYPE_ENGINE_TICK, new int(TickCount)));
}
// ------------------------------------ //
void Engine::UpdateFrameScene(){
	// update render objects //


}

void Leviathan::Engine::RenderFrame(){
	if(!Inited)
		return;

	int SinceLastFrame = -1;

	// limit check //
	if(!RenderTimer->CanRenderNow(FrameLimit, SinceLastFrame)){
		// fps would go too high //
		
		return;
	}

	// since last frame is in microseconds 10^-6 convert to milli seconds //
	SinceLastFrame /= 1000;

	FrameCount++;

	// update monitor objects //
	// advanced statistic start monitoring //
	RenderTimer->RenderingStart();

	MainEvents->CallEvent(new Event(EVENT_TYPE_FRAME_BEGIN, new int(SinceLastFrame)));

	// Gui object animations //
	Gui->AnimationTick(SinceLastFrame);

	//Logger::Get()->Info(L"RendTime: "+Convert::IntToWstring(SinceRender), false);

	// update simulations will go here, or not //
#ifdef _DEBUG
	// some test magic here //

	// update model stuff //
	shared_ptr<BaseObject> bobj = GObjects->Get(0); // nasty index grab //
	if(bobj.get() != NULL){
		if(bobj->Type == OBJECT_TYPE_MODEL){
			GameObject::Model* modl = dynamic_cast<GameObject::Model*>(bobj.get());
			// update some shit //
			//modl->SetScale(modl->GetScale()*(1.0001f/**SinceRender*/));
			//if(TickCount < 186)
			//	modl->SetOrientation(modl->GetPitch()+5*SinceLastFrame, modl->GetYaw()+4*SinceLastFrame, modl->GetRoll()+7*SinceLastFrame);


		}
	}

#endif

	// Gui //
	Gui->Render();
	// render //
	vector<BaseRenderable*>* objects = GObjects->GetRenderableObjects();
	Graph->Frame(SinceLastFrame, MainCamera, *objects);

	MainEvents->CallEvent(new Event(EVENT_TYPE_FRAME_END, new int(FrameCount)));

	// advanced statistics frame has ended //
	RenderTimer->RenderingEnd();
}
// ------------------------------------ //
bool Engine::HandleWindowCallBack(UINT message, WPARAM wParam,LPARAM lParam){
	return false;
}
bool Engine::DoWindowResize(int width, int height){
	// tell window class to resize the real windows window //
	Wind->ResizeWin32Window(width, height);

	// update values //
	this->OnResize(width, height);
	return true;
}
void Engine::OnResize(int width, int height){
	// skip if width and height are the same //
	if((Mainstore->GetWidth() == width) && (Mainstore->GetHeight() == height)){
		return;
	}

	// update resolution scaling //
	ResolutionScaling::SetResolution(width, height);

	// update values in DataStore and call resize functions of things //
	Mainstore->SetWidth(width);
	Mainstore->SetHeight(height);
	// set them also to window class //
	Wind->SetNewSize(width, height);

	// update rendering stuff //
	Graph->Resize(width, height);


	// update Gui //
	Gui->OnResize();
}
// ------------------------------------ //
 void Engine::CaptureMouse(bool toset){
	 MouseCaptured = toset;
	 Wind->SetHideCursor(toset);
	 Inputs->SetMouseCapture(toset);
	 WantsToCapture = toset;
 }
 void Engine::SetGuiActive(bool toset){
	 GuiActive = toset;
	 if(GuiActive){
		CaptureMouse(false);

	 } else {
		 Wind->SetMouseToCenter();
		 CaptureMouse(true);
	 }
	 Mainstore->SetGUiActive(GuiActive);

 }

void Engine::LoseFocus(){
	Wind->LoseFocus();
	Inputs->SetMouseCapture(false);
	Focused = false;
}
void Engine::GainFocus(){
	Wind->GainFocus();
	Focused = true;
	if(!GuiActive){
		Inputs->SetMouseCapture(true);
	}

}

// ------------------------------------ //
Engine* Engine::GetEngine(){
	return instance;
}
Engine* Engine::instance = NULL;
// ------------------------------------ //
void Engine::ExecuteCommandLine(wstring commands){
	bool incommand = false;
	bool inparams = false;

	bool execute = false;

	wstring command;
	wstring params;

	for(unsigned int i = 0; i < commands.size(); i++){
		if(execute){
			// run command //
			incommand = false;
			inparams = false;
			execute = false;

			RunScrCommand(command, params);

			command = L"";
			params = L"";
		}
		if(incommand){
			if(inparams){
				if(commands[i] == L' '){
					execute = true;
				} else if(commands[i] == L'-'){
					incommand = true;
					inparams = false;
					execute = false;
					RunScrCommand(command, params);

					command = L"";
					params = L"";
					continue;

				} else {
					params += commands[i];
				}
				continue;
			}
			if(commands[i] == L' '){
				inparams = true;
			} else {
				command += commands[i];
			}
			continue;

		} else {
			if(commands[i] == L'-')
				incommand = true;
		}


	}
	if(command != L""){
		RunScrCommand(command, params);
	}

}
void Engine::RunScrCommand(wstring command, wstring params){
#ifdef _DEBUG
	 Mainlog->Info(L"[DEBUG] Running script command: "+command+L" with params "+params, false);
#endif
	 if(Misc::WstringCompareInsensitive(command, L"Print")){
		Mainlog->Info(params, false);
		return;
	 }
}
// ------------------------------------ //
RenderingLight* Engine::GetLightAtObject(BasePositionable* obj){
	// just return basic light //
	return Graph->Light;

}



// ------------------------------------ //

void Engine::AddObject(BaseObject* obj){
	GObjects->AddObject(obj);
}

const shared_ptr<BaseObject>& Engine::GetObjectByID(int id){
	return GObjects->Search(id);
}

int Engine::GetIndex(int id){
	return GObjects->SearchGetIndex(id);
}

const shared_ptr<BaseObject>& Engine::GetObjectByIndex(int index){
	return GObjects->Get(index);
}

void Engine::RemoveObject(int id){
	GObjects->Delete(id);
}

bool Engine::RemoveObjectByIndex(int index){
	// not implemented in Gobjects //
	return false;
}

int Engine::GetObjectCount(){
	return GObjects->GetObjectCount();
}
// ------------------------------------ //