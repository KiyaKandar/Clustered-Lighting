#include "Bloom.h"

Bloom::Bloom(Vector2 resolution) : GSetting(resolution)
{
	//bloomShader		= new Shader(SHADERDIR"/Bloom/bloomvert.glsl",		SHADERDIR"/Bloom/bloomfrag.glsl",	"", true);
	blurShader		= new Shader(SHADERDIR"/Bloom/blurvert.glsl",		SHADERDIR"/Bloom/blurfrag.glsl");
	combineShader	= new Shader(SHADERDIR"/Bloom/combinevert.glsl",	SHADERDIR"/Bloom/combinefrag.glsl");
}

Bloom::~Bloom()
{
	//delete bloomShader;
	delete blurShader;
	delete combineShader;
}

void Bloom::LinkShaders()
{
	blurShader->LinkProgram();
	combineShader->LinkProgram();
}

void Bloom::RegenerateShaders()
{
	blurShader->Regenerate();
	combineShader->Regenerate();
}

void Bloom::Initialise()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Brightness texture
	glGenTextures(2, colourBuffers);
	for (unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colourBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourBuffers[i], 0);

	}

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.x, resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(2, attachments);
	CheckBuffer("RBO DEPTH");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Blur texture
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColourBuffers);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColourBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColourBuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)

		CheckBuffer("Bloom blur");
	}

	LocateUniforms();
}

void Bloom::LocateUniforms() 
{
	loc_horizontal	= glGetUniformLocation(blurShader->GetProgram(),	"horizontal");
	loc_image		= glGetUniformLocation(blurShader->GetProgram(),	"image");


	loc_scene		= glGetUniformLocation(combineShader->GetProgram(), "scene");
	loc_bloomBlur	= glGetUniformLocation(combineShader->GetProgram(), "bloomBlur");
}

void Bloom::Apply()
{
	glEnable(GL_DEPTH_TEST);

	horizontal		= true;
	first_iteration = true;

	ApplyBlur();
	Combine();
}

void Bloom::ApplyBlur()
{
	GLuint strength = 100;
	SetCurrentShader(blurShader);

	for (GLuint i = 0; i < strength; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(loc_horizontal, horizontal);

		glUniform1i(loc_image, IMAGE);

		//Bind texture of other framebuffer (or scene if first iteration)
		glActiveTexture(GL_TEXTURE0 + IMAGE);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colourBuffers[1] : 
			pingpongColourBuffers[!horizontal]);  

		RenderScreenQuad();

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::Combine()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetCurrentShader(combineShader);

	glUniform1i(loc_scene, SCENE);
	glUniform1i(loc_bloomBlur, BLOOM_BLUR);

	glActiveTexture(GL_TEXTURE0 + SCENE);
	glBindTexture(GL_TEXTURE_2D, colourBuffers[0]);

	glActiveTexture(GL_TEXTURE0 + BLOOM_BLUR);
	glBindTexture(GL_TEXTURE_2D, pingpongColourBuffers[!horizontal]);

	RenderScreenQuad();
}
