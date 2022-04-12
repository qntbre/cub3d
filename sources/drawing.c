#include "../headers/cub3d.h"

unsigned int get_pixel_color(int x, int y, char *firstpixel)
{
	unsigned int *fp;;

	fp = (unsigned int *)firstpixel;
	return (fp[y * 64 + x]);
}

void    set_pixel_color(int x, int y, char *firstpixel, unsigned int newVal)
{
	unsigned int *fp;;

	fp = (unsigned int *)firstpixel;
	fp[y * 64 + x] = newVal;
}


int *load_line_texture(int len, int step, int texPos, int texX, int side, t_image *texture)
{
	int        *lineText;
	int        i;
	int        texY;
	uint32_t   color;

	i = 0;
	lineText = malloc(sizeof(int) * (len + 1));
	while(i < len)
	{
		texY = (int)texPos & (64 - 1);
		texPos += step;
		color = get_pixel_color(texX, texY, texture->addr);
		if(side == 1)
			color = (color >> 1) & 8355711;
		lineText[i] = color;
		i++;
	}
	lineText[i] = 0;
	return (lineText);
}

t_image *get_texture(char c, t_assets *text)
{
	if (c == 'W')
		return (text->W_texture);
	else if (c == 'E')
		return (text->E_texture);
	else if (c == 'N')
		return (text->N_texture);
	else if (c == 'S')
		return (text->S_texture);
	else
		return (NULL);
}

void ft_temp(int drawStart, int y,char c, double step, int drawEnd, int side, uint32_t *textswag, int texX, double *texPos, t_image *text, int texY)
{

    uint32_t color = get_pixel_color(texX, texY, text->addr);

    if(side == 1) {
        color = (color >> 1) & 8355711;
    }
    *textswag = color;
    *texPos += step;//THIS|||
//    if(side == 1) {
//        *color = (*color >> 1) & 8355711;
//    }
//
//    textswag[y - drawStart] = *color;
   //int *textLine = load_line_texture(lineHeight, step, texPos, texX, side, texture);
    /*ft_verline(i, drawStart, game, textLine, drawEnd - drawStart);*/

//        if(side == 1)
//            *color = (*color >> 1) & 8355711;
//        //buffer[y][i] = color;
//        *textswag[y - drawStart] = *color;
        //my_mlx_pixel_put(game->buffer, y, 0, color);
    //}
    //textswag[y - drawStart] = 0;
}

t_image *get_ray_texture(t_game *game, int side, double rayDirX, double rayDirY)
{
	if(side)
	{
		if(rayDirY < 0)
			return (get_texture('N', game->textures));
		else
			return (get_texture('S', game->textures));
	}
	else
	{
		if(rayDirX < 0)
			return (get_texture('W', game->textures));
		else
			return (get_texture('E', game->textures));
	}
}

void drawRays3D(void *g)
{
	int i = 0;
	double sideDistX;
	double sideDistY;
	double deltaDistX;
	double deltaDistY;
	t_game *game = (t_game *)g;

	i = 0;
	do_action(game);
	while (i < WINDOW_WIDTH)
	{
		double cameraX = 2 * (i - WINDOW_WIDTH/2) / ((double)(WINDOW_WIDTH)-1); // x-coordinate in camera space
		double rayDirX = game->camera->dirX + game->camera->planeX * cameraX;
		double rayDirY = game->camera->dirY + game->camera->planeY * cameraX;
		int mapx = (int)game->player->posX;
		int mapy = (int)game->player->posY;
		deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
		deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
		//exit(0);
		double perpWallDist;

		// what direction to step in x or y-direction (either +1 or -1)
		double stepX;
		double stepY;
		int hit = 0; // was there a wall hit?
		int side;	 // was a NS or a EW wall hit?
		// calculate step and initial sideDist
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (game->player->posX - mapx) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapx + 1.0 - game->player->posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (game->player->posY - mapy) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapy + 1.0 - game->player->posY) * deltaDistY;
		}
		char c;
		while (hit == 0)
		{
			// jump to next map square, either in x-direction, or in y-direction
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapx += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapy += stepY;
				side = 1;
			}
			// Check if ray has hit a wall
			if (game->map[mapy][mapx] == '1')
				hit = 1;
		}
		if (side == 0)
			perpWallDist = (sideDistX - deltaDistX);
		else
			perpWallDist = (sideDistY - deltaDistY);
		// Calculate height of line to draw on screen
		int lineHeight = (int)((WINDOW_HEIGHT) / perpWallDist);
		// calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + (WINDOW_HEIGHT) / 2;
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = lineHeight / 2 + (WINDOW_HEIGHT) / 2;
		if (drawEnd >= (WINDOW_HEIGHT))
			drawEnd = (WINDOW_HEIGHT) - 1;

		//calculate value of wallX
		double wallX; //where exactly the wall was hit
		if (side == 0)
			wallX = game->player->posY + perpWallDist * rayDirY;
		else
			wallX = game->player->posX + perpWallDist * rayDirX;
		wallX -= floor(wallX);

		//x coordinate on the texture
		int texX = (int)(wallX * (64.0));
		if((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0))
			texX = 64 - texX - 1;

		// How much to increase the texture coordinate per screen pixel
		double step = 1.0 * 64.0 / lineHeight;
		// Starting texture coordinate
		double texPos = (drawStart - WINDOW_HEIGHT / 2 + lineHeight / 2) * step;
        uint32_t *textswag = malloc(sizeof(uint32_t) * (drawEnd - drawStart + 1));
		//t_image *text = get_texture(c, game->textures);
		t_image *text = get_ray_texture(game, side, rayDirX, rayDirY);
		int y = drawStart;
        for(; y<drawEnd; y++) {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int texY = (int) texPos & (64 - 1);
            uint32_t *test = &textswag[y - drawStart];

            ft_temp(drawStart, drawStart, c, step, drawEnd, side, test, texX, &texPos, text, texY);
        }
        textswag[y - drawStart] = 0;
		//load2()
		ft_verline(i, drawStart, game, textswag, lineHeight);
        i++;
	}
	ft_draw_lifebar(game);
	//mlx_put_image_to_window(game->mlxp->mlx_ptr, game->mlxp->win_ptr, game->buffer->img, 0, 0);
}

void ft_verline(int line, int start, t_game *game, uint32_t *colors, int lineHeight)
{
	int i;
	int j;

	i = 0;
	j = 0;
	if(start < 0)
		exit(0);
	while (i < start)
	{
		my_mlx_pixel_put(game->buffer, 0, i, 0x00ff0000);
		i++;
	}
	while (colors[j])
	{
		my_mlx_pixel_put(game->buffer, 0, i, colors[j]);
		j++;
		i++;
	}
	while (i < WINDOW_HEIGHT)
	{
		my_mlx_pixel_put(game->buffer, 0, i, 0x0000ff00);
		i++;
	}
	mlx_put_image_to_window(game->mlxp->mlx_ptr, game->mlxp->win_ptr, game->buffer->img, line, 0);
	free(colors);
}