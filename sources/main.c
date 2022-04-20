#include "cub3d.h"
int g_debug = 0;

void load_tile(char tile, size_t posX, size_t posY, t_game *game)
{
	size_t i;
	size_t j;
	int color;
	t_config *conf;

	i =  0;
	j = 0;
	conf = &game->config;
	if (tile == '1')
		color = 0x00ff0000;
	else if (tile == '0')
		color = 0;
	else
		color = 0x000000ff;
	while (i < 16)
	{
		j = 0;
		while (j < 16)
		{
			my_mlx_pixel_put(&game->coll_buffer, j + (posX * 16), i + (posY * 16), color);
			j++;
		}
		i++;
	}
}

void fix_png(t_image *img)
{
    int i;
    int j;

    i = 0;
    while(i < 64)
    {
        j = 0;
        while(j < 64)
        {
            if(get_pixel_color(i, j, img->addr) == 4278190080)
                set_pixel_color(i, j, img->addr, 0x00ffffff);
            j++;
        }
        i++;
    }
}

void load_asset(t_image *asset, char *path, t_mlxp *mlxp)
{
	int a;
	int b;

	asset->img = mlx_xpm_file_to_image(mlxp->mlx_ptr, path, &a, &b);
	asset->addr =  mlx_get_data_addr(asset->img, &asset->bits_per_pixel, &asset->line_length, &asset->endian);
	fix_png(asset);
}

void	load_textures(t_game *game)
{
	game->textures.S_texture = malloc(sizeof (t_image));
	game->textures.E_texture = malloc(sizeof (t_image));
	game->textures.W_texture = malloc(sizeof (t_image));
	game->textures.N_texture = malloc(sizeof (t_image));
	load_asset(game->textures.E_texture, "textures/flag.xpm", &game->mlxp);
	load_asset(game->textures.S_texture, "textures/wall.xpm", &game->mlxp);
	load_asset(game->textures.N_texture, "textures/roz.xpm", &game->mlxp);
	load_asset(game->textures.W_texture, "textures/mac64.xpm", &game->mlxp);

}

int render_frame2D(void *g)
{
	t_game *game = (t_game *)g;
	int i = 0;
	int j = 0;

	while (game->map[i])
	{
		j = 0;
		while (game->map[i][j])
		{
			load_tile(game->map[i][j], j, i, game);
			j++;
		}
		i++;
	}
	//printf("%f %f\n", game->camera->dirX, game->camera->dirY);
	int z = 20;
	double tposx = game->player.pos.x;
	double tposy = game->player.pos.y;
	while (z)
	{
		tposx += game->camera.dirX;
		tposy += game->camera.dirY;
		my_mlx_pixel_put(&game->coll_buffer, (int)(game->player.pos.x * 15) + tposx, (int)(game->player.pos.y * 15) + tposy, 0x0000ff00);
		z--;
	}
	mlx_put_image_to_window(game->mlxp.mlx_ptr, game->mlxp.win_ptr, &game->coll_buffer.img, 0, 0);
	return (0);
}

unsigned int createRGBA(int r, int g, int b, int a)
{
	return ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void ft_draw_lifebar(t_game *game)
{
    int		j;
    int		i;

    i = 0;
    while (i < WINDOW_WIDTH / 3)
    {
        j = -1;
        while (++j < 30)
        {
            if (j < 5 || j > 25 || i < 5 || i > WINDOW_WIDTH / 3 - 5)
                my_mlx_pixel_put(&game->lifebar, i, j, 0x00ffffff);
            else if (i < game->player.health)
                my_mlx_pixel_put(&game->lifebar, i, j, 0x00ff0000);
            else
                my_mlx_pixel_put(&game->lifebar, i, j, 0x00000000);
        }
        i++;
    }
    mlx_put_image_to_window(game->mlxp.mlx_ptr, game->mlxp.win_ptr, game->lifebar.img, 10, WINDOW_HEIGHT - 100); //TODO check
}

char	get_adjacent_cardinal(int vec, char curr)
{
	char	*cardinal;
	int 	i;
	char 	ret;

	i = 0;
	cardinal = ft_strcpy("NESW");
	while (curr != cardinal[i])
			i++;
	i += vec;
	ret = cardinal[i % 4];
	free(cardinal);
	return ret;
}

int key_relase(int kc, t_game *game)
{
	if (kc == D_KEY)
		game->player.current_action[R_RIGHT_INDEX] = 0;
	if (kc == A_KEY)
		game->player.current_action[R_LEFT_INDEX] = 0;
	if (kc == W_KEY)
		game->player.current_action[FRONT_INDEX] = 0;
	if (kc == S_KEY)
		game->player.current_action[BACK_INDEX] = 0;
    return (0);
}

int get_hp(int kc, t_game *game)
{
//	if (kc == 1)
//	{
//		game->player->health += 10;
//		ft_draw_lifebar(game);
//	}
	printf("%d\n", kc);
	return (0);
}

int exit_game(int kc, t_game *game)
{
	free(game->player.current_action);
	exit(0);
}

//Default is W
void set_player_dir(t_camera *camera, char goal)
{
	double oldDirX;
	double oldPlaneX;
	char	curr;
	int 	i;

	curr = 'W';
	i = 0;
	while(curr != goal)
	{
		curr = get_adjacent_cardinal(1, curr);
		i++;
	}

	oldPlaneX = camera->planeX;
	oldDirX = camera->dirX;
	double rad = (i * -90) * (PI / 180);
	printf("i = %d rad \n", i);
	camera->dirX = oldDirX * cos(rad) - camera->dirY * sin(rad);
	camera->dirY = oldDirX * sin(rad) + camera->dirY * cos(rad);
	camera->planeX = camera->planeX * cos(rad) - camera->planeY * sin(rad);
	camera->planeY = oldPlaneX * sin(rad) + camera->planeY * cos(rad);
}

void get_player_orientation(char **map, t_config *cfg)
{
	int i;
	int j;
	char *cardi;
	char *tmp;

	i = 0;
	j = 0;
	cardi = ft_strcpy("NESW");
	while(map[i]) {
		j = 0;
		while (cardi[j])
		{
			tmp = ft_strchr(map[i], cardi[j]);
			if(tmp != NULL)
			{
				cfg->firstDir = tmp[0];
				free(cardi);
				return ;
			}
			j++;
		}
		i++;
	}
	free(cardi);
}

void	init(t_game *game)
{

}

void loop_init(t_game *game)
{
	mlx_hook(game->mlxp.win_ptr, 2, 0, key_hook, game);
	mlx_hook(game->mlxp.win_ptr, 3, 0, key_relase, game);
	mlx_hook(game->mlxp.win_ptr, 17, 0, exit_game, game);
	mlx_loop_hook(game->mlxp.mlx_ptr, (void *) draw, game);
	mlx_loop(game->mlxp.mlx_ptr);
}

void	load_and_check(t_game *game, char **argv)
{
	t_cardi_check cardiCheck;

	init_cardi_struct(&cardiCheck);
	ft_parse_file(argv[1], &cardiCheck, game);
	fill_map(&game->map);
}

void load_params(t_game *game)
{
	game->player.health = 150;
	game->camera.dirX = -1;
	game->camera.dirY = 0;
	game->player.current_action = malloc(sizeof(int) * 4);
	game->player.current_action[0] = 0;
	game->player.current_action[1] = 0;
	game->player.current_action[2] = 0;
	game->player.current_action[3] = 0;
	game->camera.planeX = 0;
	game->camera.planeY = 0.66;
	game->config.caseWidth = 16;
	game->config.mapMaxHeight = max_height(game->map);
	game->config.caseHeight = 16;
	game->config.mapMaxWidth = max_width(game->map);
	load_textures(game);
	player_setpos(game->map, &game->player);
	get_player_orientation(game->map, &game->config);
	set_player_dir(&game->camera, game->config.firstDir);
}

void	load_buffers(t_game *game)
{
	game->mlxp.mlx_ptr = mlx_init();
	game->mlxp.win_ptr = mlx_new_window(game->mlxp.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "cub3d");
	game->coll_buffer.img = mlx_new_image(game->mlxp.mlx_ptr, 1, WINDOW_HEIGHT);
	game->lifebar.img = mlx_new_image(game->mlxp.mlx_ptr, WINDOW_WIDTH / 3, 30);
	game->coll_buffer.addr = mlx_get_data_addr(game->coll_buffer.img, &game->coll_buffer.bits_per_pixel, &game->coll_buffer.line_length,
											  &game->coll_buffer.endian);
	game->lifebar.addr = mlx_get_data_addr(game->lifebar.img, &game->lifebar.bits_per_pixel, &game->lifebar.line_length,
										  &game->lifebar.endian);
}

int main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
		return (1);
	game.minimap = malloc(sizeof(t_image));
	load_and_check(&game, argv);
	load_buffers(&game);
	load_params(&game);
// TODO Comprendre pourquoi le mouse_hook fait segfault je devienne fou
	loop_init(&game);
//	mlx_mouse_hook(mlxp.win_ptr, get_hp, &game);
//	mlx_key_hook(mlxp.win_ptr, get_hp, &game);
}
