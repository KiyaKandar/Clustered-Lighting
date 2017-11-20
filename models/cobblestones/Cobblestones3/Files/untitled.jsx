#target AfterEffects

/**************************************
Scene : Scene
Resolution : 1920 x 1080
Duration : 10.416667
FPS : 24.000000
Date : 2012-07-19 01:36:17.293000
Exported with io_export_after_effects.py
**************************************/



function compFromBlender(){

var compName = prompt("Blender Comp's Name \nEnter Name of newly created Composition","BlendComp","Composition's Name");
if (compName){
var newComp = app.project.items.addComp(compName, 1920, 1080, 1.000000, 10.416667, 24);
newComp.displayStartTime = 0.083333;


// **************  CAMERA 3D MARKERS  **************


// **************  OBJECTS  **************


// **************  LIGHTS  **************


// **************  CAMERAS  **************


var _Camera = newComp.layers.addCamera("_Camera",[0,0]);
_Camera.autoOrient = AutoOrientType.NO_AUTO_ORIENT;
_Camera.property("position").setValue([842.057643,234.247770,-661.919546],);
_Camera.property("orientation").setValue([-13.355961,0.000111,0.000038],);
_Camera.property("zoom").setValue(2100.000000,);



}else{alert ("Exit Import Blender animation data \nNo Comp's name has been chosen","EXIT")};}


app.beginUndoGroup("Import Blender animation data");
compFromBlender();
app.endUndoGroup();


