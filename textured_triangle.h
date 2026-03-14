// Code credit:
// https://github.com/OneLoneCoder/Javidx9/blob/master/ConsoleGameEngine/BiggerProjects/Engine3D/OneLoneCoder_olcEngine3D_Part4.cpp
//
// License (OLC-3)
//
// Copyright 2018-2022 OneLoneCoder.com
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
// 1. Redistributions or derivations of source code must retain the above 
//    copyright notice, this list of conditions and the following disclaimer.
// 
// 2. Redistributions or derivative works in binary form must reproduce 
//    the above copyright notice. This list of conditions and the following 
//    disclaimer must be reproduced in the documentation and/or other 
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its 
//    contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
//     
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

static float fmod_1_0(float a) {
	if (a < 0) {
		return a - (int) a + 1;
	}
	else {
		return a - (int) a;
	}
}

static uint32_t sample_color(Image* image, float u, float v)
{
	//int x = (int)(fmod_1_0(u) * image->width);
	//int y = (int)(fmod_1_0(v) * image->height);
	//return image->framebuffer[y * image->width + x];
	
	return ((int)(u * 255) & 0xFF) + (((int)(v * 255) & 0xFF) << 16);
	
	
}

static void draw_pixel(int x, int y, uint32_t col)
{
	pixels[y * WIDTH + x] = col;
}

static float pDepthBuffer[WIDTH * HEIGHT];

void clear_depth_buffer()
{
	memset(pDepthBuffer, 0, sizeof pDepthBuffer);
}

static void swap(int* a, int* b) {
	int t = *a;
	*a = *b;
	*b = t;
}

static void swapF(float* a, float* b) {
	float t = *a;
	*a = *b;
	*b = t;
}

void textured_triangle(	int x1, int y1, float u1, float v1, float w1,
						int x2, int y2, float u2, float v2, float w2,
						int x3, int y3, float u3, float v3, float w3,
						Image *image)
{
	if (y2 < y1)
	{
		swap(&y1, &y2);
		swap(&x1, &x2);
		swapF(&u1, &u2);
		swapF(&v1, &v2);
		swapF(&w1, &w2);
	}

	if (y3 < y1)
	{
		swap(&y1, &y3);
		swap(&x1, &x3);
		swapF(&u1, &u3);
		swapF(&v1, &v3);
		swapF(&w1, &w3);
	}

	if (y3 < y2)
	{
		swap(&y2, &y3);
		swap(&x2, &x3);
		swapF(&u2, &u3);
		swapF(&v2, &v3);
		swapF(&w2, &w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step=0, dw2_step=0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			if (i < 0 || i >= HEIGHT) continue;
			
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(&ax, &bx);
				swapF(&tex_su, &tex_eu);
				swapF(&tex_sv, &tex_ev);
				swapF(&tex_sw, &tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				if (j < 0 || j >= WIDTH) continue;
				
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i* WIDTH + j])
				{
					draw_pixel(j, i, sample_color(image, tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i* WIDTH + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			if (i < 0 || i >= HEIGHT) continue;
			
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(&ax, &bx);
				swapF(&tex_su, &tex_eu);
				swapF(&tex_sv, &tex_ev);
				swapF(&tex_sw, &tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				if (j < 0 || j >= WIDTH) continue;
				
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i* WIDTH + j])
				{
					draw_pixel(j, i, sample_color(image, tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i* WIDTH + j] = tex_w;
				}
				t += tstep;
			}
		}	
	}		
}