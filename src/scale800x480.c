/*
   Based on AdvanceMAME Scale3x found on this page,
   http://scale2x.sourceforge.net/

*/

#include <SDL.h>
#include <assert.h>

#include "scale800x480.h"

static void scale800x480_line_triple(Uint8 *dst, int dstpitch, Uint8 *src1, Uint8 *src2, Uint8 *src3)
{
    int x;
    unsigned int delta, num_pixels;
    Uint8 A, B, C, D, E, F, G, H, I;

    delta = (50 << 24) / 19; // = 800 / 304

    // triple line
    B = C = *src1++;
    E = F = *src2++;
    H = I = *src3++;
    num_pixels = 1 << 23; // = 0.5
    for (x=0; x<303; x++)
    {
        A = B;
        D = E;
        G = H;
        B = C;
        E = F;
        H = I;
        C = *src1++;
        F = *src2++;
        I = *src3++;
        num_pixels += delta;

        if ((num_pixels >> 24) == 2)
        {
            // double column
            num_pixels -= (2 << 24);
            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = (B == F) ? F : E; // E2
                dst[dstpitch] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                dst[dstpitch+1] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                dst[2*dstpitch] = (D == H) ? D : E; // E6
                dst[2*dstpitch+1] = (H == F) ? F : E; // E8
                dst += 2;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E2
                dst[dstpitch] = E; // E3
                dst[dstpitch+1] = E; // E5
                dst[2*dstpitch] = E; // E6
                dst[2*dstpitch+1] = E; // E8
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
                dst[dstpitch] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
                dst[dstpitch+1] = E; // E4
                dst[dstpitch+2] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
                dst[2*dstpitch] = (D == H) ? D : E; // E6
                dst[2*dstpitch+1] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[2*dstpitch+2] = (H == F) ? F : E; // E8
                dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[dstpitch] = E; // E3
                dst[dstpitch+1] = E; // E4
                dst[dstpitch+2] = E; // E5
                dst[2*dstpitch] = E; // E6
                dst[2*dstpitch+1] = E; // E7
                dst[2*dstpitch+2] = E; // E8
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

        if (B != H && D != F)
        {
            dst[0] = (D == B) ? D : E; // E0
            dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
            dst[2] = (B == F) ? F : E; // E2
            dst[dstpitch] = ( (D == B && E != G) || (D == H && E != A) ) ? D : E; // E3
            dst[dstpitch+1] = E; // E4
            dst[dstpitch+2] = ((B == F && E != I) || (H == F && E != C) ) ? F : E; // E5
            dst[2*dstpitch] = (D == H) ? D : E; // E6
            dst[2*dstpitch+1] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
            dst[2*dstpitch+2] = (H == F) ? F : E; // E8
            dst += 3;
        }
        else
        {
            dst[0] = E; // E0
            dst[1] = E; // E1
            dst[2] = E; // E2
            dst[dstpitch] = E; // E3
            dst[dstpitch+1] = E; // E4
            dst[dstpitch+2] = E; // E5
            dst[2*dstpitch] = E; // E6
            dst[2*dstpitch+1] = E; // E7
            dst[2*dstpitch+2] = E; // E8
            dst += 3;
        }
    }
}

static void scale800x480_line_double(Uint8 *dst, int dstpitch, Uint8 *src1, Uint8 *src2, Uint8 *src3)
{
    int x;
    unsigned int delta, num_pixels;
    Uint8 A, B, C, D, E, F, G, H, I;

    delta = (50 << 24) / 19; // = 800 / 304

    // double line
    B = C = *src1++;
    E = F = *src2++;
    H = I = *src3++;
    num_pixels = 1 << 23; // = 0.5
    for (x=0; x<303; x++)
    {
        A = B;
        D = E;
        G = H;
        B = C;
        E = F;
        H = I;
        C = *src1++;
        F = *src2++;
        I = *src3++;
        num_pixels += delta;

        if ((num_pixels >> 24) == 2)
        {
            // double column
            num_pixels -= (2 << 24);
            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = (B == F) ? F : E; // E2
                dst[dstpitch] = (D == H) ? D : E; // E6
                dst[dstpitch+1] = (H == F) ? F : E; // E8
                dst += 2;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E2
                dst[dstpitch] = E; // E6
                dst[dstpitch+1] = E; // E8
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
                dst[dstpitch] = (D == H) ? D : E; // E6
                dst[dstpitch+1] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
                dst[dstpitch+2] = (H == F) ? F : E; // E8
                dst += 3;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2] = E; // E2
                dst[dstpitch] = E; // E6
                dst[dstpitch+1] = E; // E7
                dst[dstpitch+2] = E; // E8
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

        if (B != H && D != F)
        {
            dst[0] = (D == B) ? D : E; // E0
            dst[1] = ( (D == B && E != C) || (B == F && E != A) ) ? B : E; // E1
            dst[2] = (B == F) ? F : E; // E2
            dst[dstpitch] = (D == H) ? D : E; // E6
            dst[dstpitch+1] = ((D == H && E != I) || (H == F && E != G) ) ? H : E; // E7
            dst[dstpitch+2] = (H == F) ? F : E; // E8
            dst += 3;
        }
        else
        {
            dst[0] = E; // E0
            dst[1] = E; // E1
            dst[2] = E; // E2
            dst[dstpitch] = E; // E6
            dst[dstpitch+1] = E; // E7
            dst[dstpitch+2] = E; // E8
            dst += 3;
        }
    }
}

void scale800x480(Uint8 *dst, int dstpitch, Uint8 *src, int height)
{
    int y, odd;

    // first line (triple line)
    scale800x480_line_triple(dst, dstpitch, src, src, src+304);
    src+=304;
    dst+=3*dstpitch;

    odd = height&1;
    height = height - odd - 2;
    for (y=0; y<height; y+=2)
    {
        // double line
        scale800x480_line_double(dst, dstpitch, src-304, src, src+304);
        src+=304;
        dst+=2*dstpitch;

        // triple line
        scale800x480_line_triple(dst, dstpitch, src-304, src, src+304);
        src+=304;
        dst+=3*dstpitch;
    }

    if (odd)
    {
        // last-1 line (double line)
        scale800x480_line_double(dst, dstpitch, src-304, src, src+304);
        src+=304;
        dst+=2*dstpitch;

        // last line (triple line)
        scale800x480_line_triple(dst, dstpitch, src-304, src, src);
    }
    else
    {
        // last line (double line)
        scale800x480_line_double(dst, dstpitch, src-304, src, src);
    }
}

