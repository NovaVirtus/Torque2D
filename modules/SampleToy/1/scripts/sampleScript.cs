function buildSampleSprite(%position, %size)
{
	%sprite = new Sprite();
	%sprite.Position = %position;
	%sprite.Size = %size;
	
	%sprite.Image = "SampleToy:soccer";
	
	%sprite.setBodyType("static");
	
	%obj1 = new TestClass();  
	%obj1.EmitLight = true;  
	%obj1.Brightness = 23.4;  
  
	// I am the same!  
	%obj2 = %obj1.clone(); 

	return %sprite;
}