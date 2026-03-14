
static float fmod_1_0(float a) {
	if (a < 0) {
		return a - (int) a + 1;
	}
	else {
		return a - (int) a;
	}
}

static colour_t sample_color(texture_t* tex, float u, float v)
{
	int x = (int)(fmod_1_0(u) * tex->width);
	int y = (int)(fmod_1_0(v) * tex->height);
	return tex->pixels[y * tex->width + x];
}

static colour_t multiply_color(colour_t x, float b) {
	if (b >= 0.99f) return x;
	if (b <= 0.01f) {
		x.r = x.g = x.b = 0;
		return x;
	}
	
	x.r *= b;
	x.g *= b;
	x.b *= b;
	return x;
}

static void invert_homography(float H[3][3], float Hinv[3][3])
{
	float a = H[0][0], b = H[0][1], c = H[0][2];
	float d = H[1][0], e = H[1][1], f = H[1][2];
	float g = H[2][0], h = H[2][1], i = H[2][2];

	float det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
	if(fabs(det) < 1e-8) det = 1e-8f; // avoid divide by zero

	Hinv[0][0] =  (e*i - f*h)/det;
	Hinv[0][1] = -(b*i - c*h)/det;
	Hinv[0][2] =  (b*f - c*e)/det;
	Hinv[1][0] = -(d*i - f*g)/det;
	Hinv[1][1] =  (a*i - c*g)/det;
	Hinv[1][2] = -(a*f - c*d)/det;
	Hinv[2][0] =  (d*h - e*g)/det;
	Hinv[2][1] = -(a*h - b*g)/det;
	Hinv[2][2] =  (a*e - b*d)/det;
}

void textured_quad(
	vec3_t* quad,
	float u0, float v0,
	float u1, float v1,
	float u2, float v2,
	float u3, float v3,
	texture_t* tex,
	float brightness,
	float fog,
	char water
)
{
    float src[4][2] = { {u0,v0}, {u1,v1}, {u2,v2}, {u3,v3} };
    float dst[4][2];
    for(int k=0;k<4;k++){ dst[k][0]=quad[k].x; dst[k][1]=quad[k].y; }

    float A[8][8] = {0};
    float B[8] = {0};
    for(int i=0;i<4;i++){
        float u = src[i][0], v = src[i][1];
        float x = dst[i][0], y = dst[i][1];
        A[2*i][0]=u; A[2*i][1]=v; A[2*i][2]=1; A[2*i][3]=0; A[2*i][4]=0; A[2*i][5]=0; A[2*i][6]=-x*u; A[2*i][7]=-x*v;
        B[2*i]=x;
        A[2*i+1][0]=0; A[2*i+1][1]=0; A[2*i+1][2]=0; A[2*i+1][3]=u; A[2*i+1][4]=v; A[2*i+1][5]=1; A[2*i+1][6]=-y*u; A[2*i+1][7]=-y*v;
        B[2*i+1]=y;
    }

    float Hflat[8];
    for(int i=0;i<8;i++){
        for(int k=i;k<8;k++){ if(fabs(A[k][i])>1e-8){ if(k!=i){for(int j=0;j<8;j++){float t=A[i][j];A[i][j]=A[k][j];A[k][j]=t;} float tb=B[i];B[i]=B[k];B[k]=tb;} break; } }
        for(int j=i+1;j<8;j++){
            float f = A[j][i]/A[i][i];
            for(int k=i;k<8;k++) A[j][k]-=f*A[i][k];
            B[j]-=f*B[i];
        }
    }
    for(int i=7;i>=0;i--){
        float sum=B[i];
        for(int j=i+1;j<8;j++) sum-=A[i][j]*Hflat[j];
        Hflat[i]=sum/A[i][i];
    }
    float H[3][3] = {
        {Hflat[0], Hflat[1], Hflat[2]},
        {Hflat[3], Hflat[4], Hflat[5]},
        {Hflat[6], Hflat[7], 1.0f}
    };
    float Hinv[3][3];
    invert_homography(H,Hinv);
    float minX=WIDTH, minY=HEIGHT, maxX=0, maxY=0;
    for(int i=0;i<4;i++){
        if(dst[i][0]<minX) minX=dst[i][0];
        if(dst[i][1]<minY) minY=dst[i][1];
        if(dst[i][0]>maxX) maxX=dst[i][0];
        if(dst[i][1]>maxY) maxY=dst[i][1];
    }
    int ix0=(int)fmax(0,floor(minX));
    int iy0=(int)fmax(0,floor(minY));
    int ix1=(int)fmin(WIDTH-1,ceil(maxX));
    int iy1=(int)fmin(HEIGHT-1,ceil(maxY));

    for(int y=iy0;y<=iy1;y++){
		int x = ix0;
		if (water && (y&1)) x++;
        for(;x<=ix1;x++){
            float denom = Hinv[2][0]*x + Hinv[2][1]*y + Hinv[2][2];
            float u = (Hinv[0][0]*x + Hinv[0][1]*y + Hinv[0][2]) / denom;
            float v = (Hinv[1][0]*x + Hinv[1][1]*y + Hinv[1][2]) / denom;
            if(u>=0 && u<=1 && v>=0 && v<=1){
				colour_t c = multiply_color(sample_color(tex,u,v), brightness);
                pixels[y*WIDTH+x] = pack_colour_to_uint32(&c);
            }
			
			if (water) x++;
        }
    }
}
