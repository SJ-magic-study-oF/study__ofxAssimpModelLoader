/************************************************************
************************************************************/
#include "ofApp.h"


/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _id_Animation)
: id_Animation(_id_Animation)
, b_cam(false)
, b_DispLight(false)
{
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle( "3d loader" );
	ofSetWindowShape( WIDTH, HEIGHT );
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	camera.setDistance(600);
	
	/********************
	********************/
	img_back.load("stage.jpg");
	
	/********************
	********************/
	// model.loadModel("teapot.x");
	model.loadModel("biped.x");
	/*
	// この方法だと、.xに2つ以上Animation定義があっても、常に最後のAnimation再生となってしまった。
	model.playAllAnimations();
	model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
	*/

	// Animation部分をpointer経由で受け取り、これに対して作業を行うことで、複数のAnimation定義を使い分けることが可能.
	if(model.getAnimationCount() <= id_Animation) id_Animation = model.getAnimationCount() - 1;
	model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
	modelAnimation = &model.getAnimation(id_Animation);
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
}

/******************************
******************************/
void ofApp::setup_gui()
{
	/********************
	********************/
	gui.setup();
	
	/********************
	********************/
	{
		float thresh = 1000;
		ofVec3f position_init = ofVec3f(0, 0, 0);
		ofVec3f position_min = ofVec3f(-thresh, -thresh, -thresh);
		ofVec3f position_max = ofVec3f(thresh, thresh, thresh);
		gui.add(cam_Target.setup("cam target", position_init, position_min, position_max));
	}
	
	/********************
	********************/
	gui.add(b_DispLight.setup("b_DispLight", "true"));
	
	/********************
	********************/
	guiGp_point.setup("point light");
	{
		ofColor initColor = ofColor(0, 255, 0, 255);
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
		ofVec3f position_init = ofVec3f(0, 0, 300);
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

//--------------------------------------------------------------
void ofApp::update(){
	/********************
	modelAnimationはmodel内のobject。
	まずこれを更新し、このデータを使って親であるmodelを更新する。
	********************/
	modelAnimation->update();
	model.update();

}

/******************************
******************************/
void ofApp::apply_gui_parameter()
{
	PointLight.setPosition(point_position);
	PointLight.setDiffuseColor(ofColor(pointColor_diffuse));
	PointLight.setSpecularColor(ofColor(pointColor_specular));
	PointLight.setAttenuation(point_attenuation_constant, point_attenuation_linear, point_attenuation_quadratic);
}

//--------------------------------------------------------------
void ofApp::draw(){
	/********************
	********************/
	ofBackground(0);
	ofSetColor(255, 255, 255, 255);
	
	/********************
	********************/
	img_back.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	********************/
	ofEnableDepthTest();
	ofPushStyle();
	
	apply_gui_parameter();
	
	if(b_cam){
		/********************
		camera.setTarget(cam_Target);
			camera.begin();
			の前に入れるか、後に入れるか、で、姿勢の修正が入るか否か、が変わるので上手く使い分けること.
		********************/
		camera.setTarget(cam_Target); // 座標だけ変更
		camera.begin();
		// camera.setTarget(cam_Target); // 座標だけでなく、cameraの姿勢もxy平面に垂直に直す.
		
		ofPushMatrix();
		
			PointLight.enable();
			
			/********************
			camera.begin();
			の前でofScale()しても、
			camera.begin();によって、対象となる座標系がカメラの それとなるので、無効(のように見える)。
			********************/
			ofScale(-1, -1, 1);
			
			model.setPosition(0, 0, 0);
			
			/********************
			drawFaces()以外にしても、全て、drawFaces()と同じ描画であった。
			textureあると、ダメなのかな？
			********************/
			/*
			// drawの色々な種類
			model.drawFaces();
			model.draw(OF_MESH_FILL); //same as model.drawFaces();
			model.draw(OF_MESH_POINTS); // same as model.drawVertices();
			model.draw(OF_MESH_WIREFRAME); // same as model.drawWireframe();
			*/
			model.drawFaces();
			
			PointLight.disable();
			ofDisableLighting();
			
			/********************
			light.draw();
			lightの有効エリア外に出さないと、shadingがかかった結果、自分自身に光があたっていないので、見えない。。
			********************/
			if(b_DispLight) PointLight.draw();
			
		ofPopMatrix();
		camera.end();
	
	}else{
		ofPushMatrix();
			ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
			ofScale(-1, 1, 1);
			
			model.setPosition(0, 0, 0);
			
			/********************
			drawFaces()以外にしても、全て、drawFaces()と同じ描画であった。
			textureあると、ダメなのかな？
			********************/
			/*
			// drawの色々な種類
			model.drawFaces();
			model.draw(OF_MESH_FILL); //same as model.drawFaces();
			model.draw(OF_MESH_POINTS); // same as model.drawVertices();
			model.draw(OF_MESH_WIREFRAME); // same as model.drawWireframe();
			*/
			model.drawFaces();
		ofPopMatrix();
	}
	
	ofPopStyle();
	ofDisableDepthTest();
	
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
		case '1':
		case '2':
		{
			int id_temp = key - '0';
			
			// Animation部分をpointer経由で受け取り、これに対して作業を行うことで、複数のAnimation定義を使い分けることが可能.
			if(id_temp < model.getAnimationCount()){
				id_Animation = id_temp;
				
				modelAnimation->stop();
				modelAnimation = &model.getAnimation(id_Animation);
				modelAnimation->play();
			}
		}
			break;
			
		case 'c':
			b_cam = !b_cam;
			
			if(b_cam)	printf("cam\n");
			else		printf("no cam\n");
			
			break;
			
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
