#ifdef GL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
uniform vec2 resolution;

void main()
{
	vec4 normalColor = texture2D(CC_Texture0, v_texCoord).rgba;

    vec2 onePixel = vec2(1.0 / resolution.x, 1.0 / resolution.y);

    vec2 texCoord = v_texCoord;

	vec4 neighbor = vec4(1.0, 1.0, 1.0, 1.0);
	float dx = 1.0 / resolution.x;
	float dy = 1.0 / resolution.y;
	
	bool includeThis = false;
	
	int step = 4;
	//int xSt = -1;
	int xSt = 0;
	int xFn = xSt + step; // +
	//int ySt = -1;
	int ySt = 0;
	int yFn = ySt + step; // -
	
	for (int i = xSt; i <= xFn; ++i)
	{
		for (int j = ySt; j <= yFn; ++j)
		{
			neighbor = texture2D(CC_Texture0, vec2(texCoord.x + dx * i, texCoord.y + dy * j));
			if (neighbor.rgb == vec3(1.0)) 
			{
				includeThis = true;
				break;
			}
		}
	}
	
    vec4 color;
	
	if (includeThis == true)
	{
		color.rgb = vec3(0.5);
		color += texture2D(CC_Texture0, texCoord - onePixel * 1.0);// * 5.0;
		color -= texture2D(CC_Texture0, texCoord + onePixel * 1.0);// * 5.0;

		color.rgb = vec3((color.r + color.g + color.b) / 3.0);
			
		color.r = color.r * 233.0/255.0;
		color.g = color.g * 188.0/255.0;
		color.b = color.b * 69.0/255.0;
				
		color.a = normalColor.a;
		
		gl_FragColor = color;
	}
	else
	{
		if (normalColor.a == 1.0)
		{
			normalColor.a = 0.8;
		}
		gl_FragColor = normalColor;
	}
}