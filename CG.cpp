/************************************************************
************************************************************/
#include "CG.h"

/************************************************************
************************************************************/

/******************************
******************************/
CG::CG()
: id_Animation(0)
, DrawType(DRAW_TYPE__FACE)
, b_DispGui(true)
, b_cam_orbit(false)
, b_paused(false)
{
}

/******************************
******************************/
CG::~CG()
{
}

/******************************
******************************/
void CG::setup(){
	/********************
	********************/
	// camera.setDistance(600);
	camera.setDistance(ofGetWidth() * 0.5);

	/********************
	********************/
	char FileName[BUF_SIZE];
	// sprintf(FileName, "../../../data/3dCg/Biped.X");
	// sprintf(FileName, "../../../data/3dCg/Biped_x_Model(physique).x");
	sprintf(FileName, "../../../data/3dCg/test.x");
	
	check_if_FileExist(FileName);
	model.loadModel(FileName);
	
	
	/*
	// この方法だと、.xに2つ以上Animation定義があっても、常に最後のAnimation再生となってしまった。
	model.playAllAnimations();
	model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
	*/
	
	/*
		model.getAnimationCount();
			return is -> model.getNumMeshes() * NumAnimations
	*/
	NUM_ANIMATIONS = model.getAnimationCount() / model.getNumMeshes();
	printf("> 3D CG\n");
	printf("Num Meshes = %d, Num Animations = %d, NUM_ANIMATIONS = %d\n", model.getNumMeshes(), model.getAnimationCount(), NUM_ANIMATIONS);
	id_Animation = 0;
	/********************
	enum ofLoopType{
		/// \brief Plays the video once without looping.
		OF_LOOP_NONE=0x01,
		/// \brief Plays the video forwards then backwards repeatedly.
		OF_LOOP_PALINDROME=0x02,
		/// \brief Repeats the video over and over.
		OF_LOOP_NORMAL=0x03
	};
	********************/
	model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
	modelAnimation = &model.getAnimation(id_Animation); // Animation部分をpointer経由で受け取り、これに対して作業を行うことで、複数のAnimation定義を使い分けることが可能.
	modelAnimation->play();

	/********************
	以下は不要だった.
	********************/
	// model.enableTextures();
	// model.enableMaterials();
	// model.enableColors();
	
	/********************
	********************/
	setup_gui();
	
	PointLight.setPointLight();
	
	/********************
	********************/
	fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA);
}

/******************************
******************************/
void CG::check_if_FileExist(char* FileName)
{
	FILE* fp;
	fp = fopen(FileName, "rb");
	
	if(fp == NULL){
		ERROR_MSG();
		ofExit();
	}else{
		fclose(fp);
	}
}

/******************************
******************************/
void CG::setup_gui()
{
	/********************
	********************/
	gui.setup();
	
	/********************
	********************/
	{
		float thresh = 2000;
		ofVec3f position_init = ofVec3f(0, 285, 0);
		ofVec3f position_min = ofVec3f(-thresh, -thresh, -thresh);
		ofVec3f position_max = ofVec3f(thresh, thresh, thresh);
		gui.add(cam_Target.setup("cam target", position_init, position_min, position_max));
	}
	
	guiGp_CamOrbit.setup("cam orbit");
	{
		guiGp_CamOrbit.add(cam_Speed.setup("cam speed", 20, 10, 100));
		guiGp_CamOrbit.add(cam_latitude.setup("latitude", 0, -90.0, 90.0));
		guiGp_CamOrbit.add(cam_radius.setup("radius", 0.65, 0.2, 2.0));
	}
	gui.add(&guiGp_CamOrbit);
	
	/********************
	********************/
	gui.add(b_DispLight.setup("b_DispLight", "true"));
	
	/********************
	********************/
	guiGp_point.setup("point light");
	{
		ofColor initColor = ofColor(0, 170, 255, 255);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		guiGp_point.add(pointColor_diffuse.setup("point color diffuse", initColor, minColor, maxColor));
	}
	{
		ofColor initColor = ofColor(255, 0, 0, 255);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		guiGp_point.add(pointColor_specular.setup("point color specular", initColor, minColor, maxColor));
	}
	{
		float thresh = 1000;
		ofVec3f position_init = ofVec3f(0, -100, 500);
		ofVec3f position_min = ofVec3f(-thresh, -thresh, -thresh);
		ofVec3f position_max = ofVec3f(thresh, thresh, thresh);
		guiGp_point.add(point_position.setup("point position", position_init, position_min, position_max));
	}
	{
		guiGp_point.add(point_attenuation_constant.setup("attenuation constant", 1.0, 0.0, 1.0));
		guiGp_point.add(point_attenuation_linear.setup("attenuation linear", 0.0, 0.0, 0.1));
		guiGp_point.add(point_attenuation_quadratic.setup("attenuation quadratic", 0.0, 0.0, 0.01));
	}
	gui.add(&guiGp_point);
}


/******************************
******************************/
void CG::update(){
	/********************
	modelAnimationはmodel内のobject。
	まずこれを更新し、このデータを使って親であるmodelを更新する。
	********************/
	modelAnimation->update();
	model.update();

}

/******************************
******************************/
void CG::apply_gui_parameter()
{
	/********************
	PointLight.setPosition(point_position);
		挿入位置によらず、
			PointLight.enable();
		によってLightが有効な状態で描画される空間座標上における位置をset.
		cam空間であれば、正の方向は
			x:右
			y:上
			z:手前
		ここでは、cam空間でさらに、ofScale(-1, -1, 1);されているので
			x:左
			y:下
			z:手前
	********************/
	PointLight.setPosition(point_position);
	
	PointLight.setDiffuseColor(ofColor(pointColor_diffuse));
	PointLight.setSpecularColor(ofColor(pointColor_specular));
	PointLight.setAttenuation(point_attenuation_constant, point_attenuation_linear, point_attenuation_quadratic);
}

/******************************
******************************/
void CG::cam_orbit_motion()
{
	if(b_cam_orbit){
		float time = ofGetElapsedTimef();
		float longitude = cam_Speed * time; // rotation speed
		
		// float latitude = ofGetWidth() * 0.013 * sin(time*0.8); // y方向に軽く揺らす.
		float latitude = cam_latitude;
		
		// float radius = ofGetWidth() * 0.4 + ofGetWidth() * 0.04 * sin(time*0.4);
		float radius = ofGetWidth() * cam_radius;
		
		/********************
		camera.orbit( longitude, latitude, radius, cam_Target );
		だと、cam_Targetを中心としてlongitude, latitudeを定義するだけで、
		あくまで視点の先をsetするのは、setTarget();
		********************/
		camera.orbit( longitude, latitude, radius, ofPoint(0,0,0) );
	}
}

/******************************
座標:max->openframeworks
	実験的に、以下の通りとなった。
		no cam
			ofScale(-1, 1, 1);
			
		cam
			ofScale(-1, -1, 1);

description
	fboを介してdraw.
	Alphaも、もちろんOKなので、CGの重ね描き OK!!
******************************/
void CG::draw(double Cg_a){
	/********************
	********************/
	ofPushStyle();
	fbo.begin();
	
		/********************
		********************/
		ofBackground(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		ofDisableAlphaBlending();
	
		/********************
		********************/
		ofEnableDepthTest();
		
		apply_gui_parameter();
		
		
		/********************
		camera.setTarget(cam_Target);
			挿入位置によらず、cam座標空間でTarget位置を指定。
			つまり、正の方向は、
				x:右
				y:上
				z:手前
			
			また、camera内の映像オブジェクトの動きについて、targetを左にズラすと、オブジェクトは右にズレる点に注意。
			
			camera.orbit();
			の後に記述しないと、この関数でTargetが原点に戻されてしまうようだ。
		********************/
		if(b_cam_orbit)	cam_orbit_motion();
		
		camera.setTarget(cam_Target);
		camera.begin();
		
		ofPushMatrix();
		
			PointLight.enable();
			
			/********************
			camera.begin();
			の前でofScale()しても、
			camera.begin();によって、対象となる座標系がカメラの それとなるので、無効(のように見える)。
			
			正の方向
				x:左
				y:下
				z:手前
			********************/
			ofScale(-1, -1, 1);
			
			model.setPosition(0, 0, 0);
			
			/********************
			ofxAssimpModelLoaderに
				drawVertices();
				drawWireframe();
			が効かないバグがあったので、of_v0.9.3_osx_release
			に置き換えた。
			********************/
			switch(DrawType){
				case DRAW_TYPE__FACE:
					model.draw(OF_MESH_FILL); //same as model.drawFaces();
					break;
					
				case DRAW_TYPE__POINTS:
					model.draw(OF_MESH_POINTS); // same as model.drawVertices();
					break;
					
				case DRAW_TYPE__WIREFRAME:
					model.draw(OF_MESH_WIREFRAME); // same as model.drawWireframe();
					break;
			}
			
			PointLight.disable();
			ofDisableLighting();
			
			/********************
			light.draw();
			lightの有効エリア外に出さないと、shadingがかかった結果、自分自身に光があたっていないので、見えない。。
			********************/
			if(b_DispLight) PointLight.draw();
			
		ofPopMatrix();
		camera.end();
	
		ofDisableDepthTest();
		
	fbo.end();
	ofPopStyle();
	
	/********************
	********************/
	ofPushStyle();
		ofSetColor(255, 255, 255, int(255 * Cg_a));
		ofEnableAlphaBlending();
		fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	ofPopStyle();

	
	/********************
	********************/
	if(b_DispGui) gui.draw();
}

/******************************
******************************/
void CG::keyPressed(int key)
{
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			int id_Animation_temp = key - '0';
			if(id_Animation_temp < NUM_ANIMATIONS){
				id_Animation = id_Animation_temp;
				
				modelAnimation->stop();
				modelAnimation = &model.getAnimation(id_Animation); // Animation部分をpointer経由で受け取り、これに対して作業を行うことで、複数のAnimation定義を使い分けることが可能.
				modelAnimation->play();
			}
		}
			break;
			
		case 'c':
			b_cam_orbit = !b_cam_orbit;
			
			if(!b_cam_orbit){
				camera.reset();
				camera.setDistance(ofGetWidth() * 0.5);
			}
			break;
			
		case 'd':
			b_DispGui = !b_DispGui;
			break;
			
		case 'f':
			DrawType = DRAW_TYPE__FACE;
			break;
			
		case 'p':
			DrawType = DRAW_TYPE__POINTS;
			break;
			
		case 's':
			modelAnimation->play(); // start from Top frame.
			break;
			
		case 't':
			modelAnimation->stop();
			break;
			
		case 'u':
			b_paused = !b_paused;
			modelAnimation->setPaused(b_paused);
			break;
			
			
		case 'w':
			DrawType = DRAW_TYPE__WIREFRAME;
			break;
			
		case ' ':
		{
			ofImage image;
			image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			image.saveImage("screen.png");
		}
			break;
	}
	
	printf("return to Normal key\n");
}
