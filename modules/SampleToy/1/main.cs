function SampleToy::create(%this)
{
	exec("./scripts/sampleScript.cs");
	echo("### SampleToy::create function called!");
	SampleToy.mySpriteSize = 5;
	addNumericOption("Sprite size", 1, 10, 1, "setMySpriteSize", %this.mySpriteSize, true, "Determines the size of the sprite");

	SampleToy.reset();
}

function SampleToy::destroy(%this)
{
	echo("### SampleToy::destroy function called!");
}

function SampleToy::reset(%this)
{
	SandboxScene.clear();
	SandboxScene.setGravity(0, 0);
	%size = %this.mySpriteSize SPC %this.mySpriteSize;
	
	// Create a sprite
	%position = "0 0";
	%sprite = buildSampleSprite(%position, %size);
	
	// Add to scene
	SandboxScene.add(%sprite);
}

function SampleToy::setMySpriteSize(%this, %value)
{
	%this.mySpriteSize = %value;
}
