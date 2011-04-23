/*
   Based on AdvanceMAME Scale3x found on this page,
   http://scale2x.sourceforge.net/

*/

#include <SDL.h>
#include <assert.h>

#include "scale800x480.h"

void scale800x480(Uint8 *dst, Uint8 *src)
{
	int x, y;
    unsigned int delta, num_pixels;
    Uint8 A, B, C, D, E, F, G, H, I;

    delta = (50 << 24) / 19; // = 800 / 304

    // first line (triple line)
    {
        B = C = src[0];
        E = F = src[0];
        H = I = src[304];
        num_pixels = 1 << 23; // = 0.5
        for (x=0; x<303; x++)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            C = src[1];
            F = src[1];
            I = src[305];
            src++;
            num_pixels += delta;

            if ((num_pixels >> 24) == 2)
            {
                // double column
                num_pixels -= (2 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = (B == F) ? F : E; // E2
                    dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                    dst[801] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                    dst[1600] = (D == H) ? D : E; // E6
                    dst[1601] = (H == F) ? F : E; // E8
                    dst += 2;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E2
                    dst[800] = E; // E3
                    dst[801] = E; // E5
                    dst[1600] = E; // E6
                    dst[1601] = E; // E8
                    dst += 2;
                }
            }
            else
            {
                // triple column
                num_pixels -= (3 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                    dst[2] = (B == F) ? F : E; // E2
                    dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                    dst[801] = E; // E4
                    dst[802] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                    dst[1600] = (D == H) ? D : E; // E6
                    dst[1601] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                    dst[1602] = (H == F) ? F : E; // E8
                    dst += 3;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E1
                    dst[2] = E; // E2
                    dst[800] = E; // E3
                    dst[801] = E; // E4
                    dst[802] = E; // E5
                    dst[1600] = E; // E6
                    dst[1601] = E; // E7
                    dst[1602] = E; // E8
                    dst += 3;
                }
            }
        }
        // last column (triple column)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                dst[2] = (B == F) ? F : E; // E2
                dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                dst[801] = E; // E4
                dst[802] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                dst[1600] = (D == H) ? D : E; // E6
                dst[1601] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[1602] = (H == F) ? F : E; // E8
                dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[800] = E; // E3
                dst[801] = E; // E4
                dst[802] = E; // E5
                dst[1600] = E; // E6
                dst[1601] = E; // E7
                dst[1602] = E; // E8
                dst += 3;
            }
        }
        dst += 2*800;
    }

	for (y=0; y<190; y+=2)
	{
        // double line
        B = C = src[-304];
        E = F = src[0];
        H = I = src[304];
        num_pixels = 1 << 23; // = 0.5
        for (x=0; x<303; x++)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            C = src[-303];
            F = src[1];
            I = src[305];
            src++;
            num_pixels += delta;

            if ((num_pixels >> 24) == 2)
            {
                // double column
                num_pixels -= (2 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = (B == F) ? F : E; // E2
                    dst[800] = (D == H) ? D : E; // E6
                    dst[801] = (H == F) ? F : E; // E8
                    dst += 2;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E2
                    dst[800] = E; // E6
                    dst[801] = E; // E8
                    dst += 2;
                }
            }
            else
            {
                // triple column
                num_pixels -= (3 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                    dst[2] = (B == F) ? F : E; // E2
                    dst[800] = (D == H) ? D : E; // E6
                    dst[801] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                    dst[802] = (H == F) ? F : E; // E8
                    dst += 3;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E1
                    dst[2] = E; // E2
                    dst[800] = E; // E6
                    dst[801] = E; // E7
                    dst[802] = E; // E8
                    dst += 3;
                }
            }
        }
        // last column (triple column)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                dst[2] = (B == F) ? F : E; // E2
                dst[800] = (D == H) ? D : E; // E6
                dst[801] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[802] = (H == F) ? F : E; // E8
                dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[800] = E; // E6
                dst[801] = E; // E7
                dst[802] = E; // E8
                dst += 3;
            }
        }
        dst += 800;

        // triple line
        B = C = src[-304];
        E = F = src[0];
        H = I = src[304];
        num_pixels = 1 << 23; // = 0.5
        for (x=0; x<303; x++)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            C = src[-303];
            F = src[1];
            I = src[305];
            src++;
            num_pixels += delta;

            if ((num_pixels >> 24) == 2)
            {
                // double column
                num_pixels -= (2 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = (B == F) ? F : E; // E2
                    dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                    dst[801] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                    dst[1600] = (D == H) ? D : E; // E6
                    dst[1601] = (H == F) ? F : E; // E8
                    dst += 2;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E2
                    dst[800] = E; // E3
                    dst[801] = E; // E5
                    dst[1600] = E; // E6
                    dst[1601] = E; // E8
                    dst += 2;
                }
            }
            else
            {
                // triple column
                num_pixels -= (3 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                    dst[2] = (B == F) ? F : E; // E2
                    dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                    dst[801] = E; // E4
                    dst[802] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                    dst[1600] = (D == H) ? D : E; // E6
                    dst[1601] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                    dst[1602] = (H == F) ? F : E; // E8
                    dst += 3;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E1
                    dst[2] = E; // E2
                    dst[800] = E; // E3
                    dst[801] = E; // E4
                    dst[802] = E; // E5
                    dst[1600] = E; // E6
                    dst[1601] = E; // E7
                    dst[1602] = E; // E8
                    dst += 3;
                }
            }
        }
        // last column (triple column)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                dst[2] = (B == F) ? F : E; // E2
                dst[800] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                dst[801] = E; // E4
                dst[802] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                dst[1600] = (D == H) ? D : E; // E6
                dst[1601] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[1602] = (H == F) ? F : E; // E8
                dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[800] = E; // E3
                dst[801] = E; // E4
                dst[802] = E; // E5
                dst[1600] = E; // E6
                dst[1601] = E; // E7
                dst[1602] = E; // E8
                dst += 3;
            }
        }
        dst += 2*800;
    }

    // last line (double line)
    {
        B = C = src[-304];
        E = F = src[0];
        H = I = src[0];
        num_pixels = 1 << 23; // = 0.5
        for (x=0; x<303; x++)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            C = src[-303];
            F = src[1];
            I = src[1];
            src++;
            num_pixels += delta;

            if ((num_pixels >> 24) == 2)
            {
                // double column
                num_pixels -= (2 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = (B == F) ? F : E; // E2
                    dst[800] = (D == H) ? D : E; // E6
                    dst[801] = (H == F) ? F : E; // E8
                    dst += 2;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E2
                    dst[800] = E; // E6
                    dst[801] = E; // E8
                    dst += 2;
                }
            }
            else
            {
                // triple column
                num_pixels -= (3 << 24);
                if (B != H && D != F)
                {
                    dst[0] = (D == B) ? D : E; // E0
                    dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                    dst[2] = (B == F) ? F : E; // E2
                    dst[800] = (D == H) ? D : E; // E6
                    dst[801] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                    dst[802] = (H == F) ? F : E; // E8
                    dst += 3;
                }
                else
                {
                    dst[0] = E; // E0
                    dst[1] = E; // E1
                    dst[2] = E; // E2
                    dst[800] = E; // E6
                    dst[801] = E; // E7
                    dst[802] = E; // E8
                    dst += 3;
                }
            }
        }
        // last column (triple column)
        {
            A = B;
            D = E;
            G = H;
            B = C;
            E = F;
            H = I;
            //src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
                dst[2] = (B == F) ? F : E; // E2
                dst[800] = (D == H) ? D : E; // E6
                dst[801] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[802] = (H == F) ? F : E; // E8
                //dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[800] = E; // E6
                dst[801] = E; // E7
                dst[802] = E; // E8
                //dst += 3;
            }
        }
        //dst += 800;
    }
}

