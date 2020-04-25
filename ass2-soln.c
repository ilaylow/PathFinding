/* Solution to comp10002 Assignment 2, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Chuen Ley Low 1078133
   Dated:     27/9/2019

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAX 999
#define BLOCK 5
#define TRUE 1
#define FALSE 0
#define STATUS_1 10
#define STATUS_2 11
#define STATUS_3 12
#define STATUS_4 13
#define STATUS_5 14
#define STEP 4
#define INITIAL 10
#define STOP 3
#define NOFIX_STATUS 15
#define NEW_BLOCKS 20
#define GRID_MAX 9
#define NEXT 8
#define CORRECT_STATUS 5

typedef char readstr[MAX];
typedef struct node node_t;

typedef struct{
	int row, col;
} route_coords;

struct node{
	route_coords step;
	node_t *next;
};

typedef struct{
	node_t *start;
	node_t *end;
} route_t;

typedef struct{
		int rows, columns;
} dimension_t;

typedef struct{
	readstr initial, goal;
} startend_t;

typedef struct{
	route_coords repair_step;
	int numstep;
} pair;

int read_coords(char *str, int *startfrom);
int get_route(char *str, int start, route_t *route);
int check_intl(int row, int col, startend_t startend);
int check_goal(int row, int col, startend_t startend);
int check_invalid_move(int row, int col, int nextrow, int nextcol,
dimension_t dim);
int check_block(int row, int col, int **grid);
int get_pair(int **grid, dimension_t dimension, route_coords entblock,
int queue_count, pair *list_pair, int *numdistinct, route_t *route);
int in_route(int row, int col, route_t *route, route_coords entblock);
int check_if_in_queue(pair *list_pair, int row, int col, int num);
int check_move(pair *list_pair, int count, int **grid, int row, int col,
dimension_t dimension, route_coords entblock, int num, route_t *route);
int move_step(pair *list_pair, int num, int row, int col , int startstep);
int repair_route(int **grid, dimension_t dimension, route_coords *entblock,
route_t *route, int *status_msg, int pos);
void remove_newline(char *str);
void remove_old_steps(route_t *route, dimension_t dimension, int **grid);
void print_stage_0(int blocks, startend_t startend);
void get_initial(char *str, startend_t *intl);
void get_goal(char *str, startend_t *gl);
void get_dimensions(char *str, dimension_t *dimension);
void get_block(char *str, int **grid, int *blocks);
void make_empty_grid(int **grid, dimension_t *dimension);
void prt_chk_route(route_t *route, int **grid, startend_t startend,
int *status, route_coords *entblock, int print, int *pos, dimension_t dim);
void print_status(int status);
void print_grid(int **grid, dimension_t dimension, startend_t startend);
void get_repaired_route(int **grid, pair *list_pair, int num, route_t *route,
int pos);
void insert_repair(route_t *route, node_t *temp, route_coords repair);
void insert_pair(pair *list_pair, int num, int row, int col, int counter);
void delete_blocks(int **grid, dimension_t dimension);
node_t *restart_route(node_t *temp, route_coords start_point,
route_coords end_point, int get_end_node, route_t *route, int pos);
route_t *make_empty_route(void);
route_t *insert_at_end(route_t *route, route_coords step);

int main(int argc, char *argv[]) {
	int linecount = 0, getl, **grid, stopblock=0, blocks=0, print_stage_2;
	int status_msg = CORRECT_STATUS;
	size_t size = MAX;
	char *string;
	dimension_t dimension;
	startend_t startend;
	route_t *route;
	route_coords entblock;

	printf("==STAGE 0=======================================\n");
	route = make_empty_route();   /* makes an empty linked list with start, end */
	string = (char *)malloc(size); /*initialise string to read input */

	while ((getl = getline(&string, &size, stdin)) != '\n'){

		if (*string == '\n'){ /* ignore the line if it's just a newline character */
			continue;
		}
		if ((*string == '$' || getl == EOF) && stopblock){
			int pos = 0;  /* pos variable to tell us position of block in route */

			if (stopblock == TRUE){ /* stopblock TRUE indicates first set of blocks */
				print_stage_0(blocks, startend); /* prints stage 0 */

				/* function below checks route for each of the four possible status */
				prt_chk_route(route, grid, startend, &status_msg,
				&entblock, TRUE, &pos, dimension);

				printf("==STAGE 1=======================================\n");
				print_grid(grid, dimension, startend);   /* prints the grid for stage */
				stopblock = NEW_BLOCKS; print_stage_2 = TRUE;
			}

			/* stopblock NEWBLOCKS indicates new blocks */
			else if (stopblock == NEW_BLOCKS){
				if (status_msg == NOFIX_STATUS){

					prt_chk_route(route, grid, startend, &status_msg,
					&entblock, FALSE, &pos, dimension);

				}

				print_grid(grid, dimension, startend);

				prt_chk_route(route, grid, startend, &status_msg,
				&entblock, FALSE, &pos, dimension);
				/* prints grid with new blocks then checks the route with new blocks */
			}

			if (status_msg == STATUS_4){ /* if there exists a block then fix it */
				while (entblock.col != route->end->step.col ||
							entblock.row != route->end->step.row){
					/*attemps to repair route*/
					repair_route(grid, dimension, &entblock, route, &status_msg, pos);

					if (status_msg == NOFIX_STATUS || print_stage_2 == TRUE){
						break;  /*if there exists no fix, or if its only stage 1 then stop*/
					}

					prt_chk_route(route, grid, startend, &status_msg,
					&entblock, FALSE, &pos, dimension); /*checks if route is repaired*/

					if (status_msg == CORRECT_STATUS){ /* if its fixed then stop */
						break;
					}
				}
				if (status_msg != NOFIX_STATUS){
					remove_old_steps(route, dimension, grid);
					/*removes the steps in the grid that are not in the specified route*/
				}

				if (status_msg == CORRECT_STATUS){
					prt_chk_route(route, grid, startend, &status_msg,
					&entblock, FALSE, &pos, dimension);
				}
				printf("------------------------------------------------\n");

				print_grid(grid, dimension, startend); /* prints new grid */

				printf("------------------------------------------------\n");
				if (status_msg != NOFIX_STATUS){
					prt_chk_route(route, grid, startend, &status_msg,
					&entblock, TRUE, &pos, dimension);  /* prints new route */
				}
				else{
					print_status(status_msg);  /* else just print no fix available */
				}
			}
			delete_blocks(grid, dimension); /*if stage 2 then delete previous blocks*/

			if (getl == EOF || status_msg == STATUS_1 || status_msg == STATUS_2 ||
			status_msg == STATUS_3){ /* if its end of file or no fix then stop */
				printf("================================================");
				break;
			}
			else if (print_stage_2){ /* if there are new blocks then print this */

				printf("==STAGE 2=======================================\n");
				print_stage_2 = FALSE;
			}
			else{   /* else just keep printing */
				printf("================================================\n");
			}
			continue;
		}
		if (*string == '$' && !stopblock){
			stopblock = TRUE;  /* this tells program when to start reading blocks */
			continue;
		}
		remove_newline(string);
		if (linecount == 0){  /* the first line tells us the dimensions */
			int rows;

			get_dimensions(string, &dimension); /* this gets the dimensions */

			grid = (int **)malloc(dimension.rows * sizeof(int *)); /*mallocs a grid*/
			for (rows=0; rows< dimension.rows; rows++){
				grid[rows] = (int *)malloc(dimension.columns * sizeof(int));
			}
			make_empty_grid(grid, &dimension); /* all of elements of grid are zero */
		}
		else if (linecount == 1){ /* the second line tells us the initial cell */
			get_initial(string, &startend);
		}
		else if (linecount == 2){ /* third line tells us the goal cell */
			get_goal(string, &startend);
		}
		else if ((linecount > 2 && !stopblock) || stopblock == NEW_BLOCKS){
			/* gets the blocks in the grid */
			get_block(string, grid, &blocks);
		}
		else{
			int startfrom=0;
			get_route(string, startfrom, route);  /* builds a linked list of steps */
		}
		linecount++;
	}
	free(grid);        /* frees the memory of the grid and string */
	grid = NULL;
	free(string);
	string = NULL;

	return 0;
}

void print_stage_0(int blocks, startend_t startend){
	printf("The grid has %d block(s).\n", blocks);
	printf("The initial cell in the grid is %s.\n", startend.initial);
	printf("The goal cell in the grid is %s.\n", startend.goal);
	printf("The proposed route in the grid is:\n");
}

void get_dimensions(char *str, dimension_t *dimension){
	int i=0;

	dimension->rows = read_coords(str, &i);
  dimension->columns = read_coords(str, &i);
	printf("The grid has %d rows and %d columns.\n",dimension->rows,
																									dimension->columns);
}

void get_initial(char *str, startend_t *intl){
	strcpy(intl->initial, str);
}

void get_goal(char *str, startend_t *gl){
	strcpy(gl->goal, str);
}

void make_empty_grid(int **grid, dimension_t *dimension){
	/* turns all elements of grid to zero */
	int row, columns;
	for (row=0; row < dimension->rows; row++){
		for (columns=0; columns< dimension->columns; columns++){
			grid[row][columns] = 0;
		}
	}
}

void get_block(char *str, int **grid, int *blocks){
	/* functions sets grid coords to value of BLOCK, and counts num of blocks */
	int i=1;
	int coord_row, coord_col, block = *blocks;

	coord_row = read_coords(str, &i);
	coord_col = read_coords(str, &i);

	grid[coord_row][coord_col] = BLOCK;
	block++;

	*blocks = block;
}

void delete_blocks(int **grid, dimension_t dimension){
	/* functions deletes all previous blocks during stage 2 */
	int row, col;
	for (row=0; row<dimension.rows; row++){

		for (col=0; col<dimension.columns; col++){

			if (grid[row][col] == BLOCK){
				grid[row][col] = 0;
			}
		}
	}
}

int read_coords(char *str, int *startfrom){
	/* a function to read the coordinates from an input string */
	int tempi=0, start = *startfrom;
	readstr numarr;
	while (!isdigit(str[start])){
		start++;
	}

	while (isdigit(str[start])){
		numarr[tempi] = str[start];
		start++;
		tempi++;
	}
	numarr[tempi] = '\0';

	*startfrom = start;
	return atoi(numarr);
}

void remove_newline(char *str){ /* this assumes there are no whitespaces */
	int i=0;
	while (!isspace(str[i])){
		i++;
	}
	str[i] = '\0';
}

void remove_old_steps(route_t *route, dimension_t dimension, int **grid){
	/* function removes steps in grid that are not part of new route */
	node_t *temp;
	int row, col;
	route_coords start = route->start->step;
	temp = (node_t *)malloc(sizeof(*temp));
	temp = route->start;
	for (row = 0; row < dimension.rows; row++){
		for (col = 0; col< dimension. columns; col++){
			if (!in_route(row, col, route, start) && grid[row][col] == STEP){
				grid[row][col] = 0;
			}
		}
	}
}

int get_route(char *str, int start, route_t *route){
	/* function gets input of route and inserts it to the end of linked list */
	int tempi=0, startfrom=0;
	readstr routearr;
	route_coords coords;
	if (str[start] == '\0' || str[start] == EOF || start > strlen(str)){
		return 0;
	}
	else{
		while (str[start] != '-' && str[start] != '\0'){
			routearr[tempi] = str[start];
			start++;
			tempi++;
		}
		routearr[tempi] = '\0';
		startfrom = 0;

		coords.row = read_coords(routearr, &startfrom);
		coords.col = read_coords(routearr, &startfrom);

		route = insert_at_end(route, coords);
		/* recursively call the function */
		get_route(str, start + 2, route); /* plus 2 here to skip pass -> notation */
	}
	return 0;
}

route_t *make_empty_route(void){
	/* function is based off of make_empty_list function in listops.c, credit
	goes to Prof. Alistair Moffat.*/
	/* initialises an empty linked list with start and end */

	route_t *route;
	route = (route_t *)malloc(sizeof(*route));
	route->start = route->end = NULL;
	return route;
}

route_t *insert_at_end(route_t *route, route_coords step){
	/* function is based off of insert_at_foot function in listops.c, credit
	goes to Prof. Alistair Moffat.*/
	/* function to insert a new node at the end of our route linked list */

	node_t *new;
	new = (node_t *)malloc(sizeof(*new));
	assert(route!=NULL && new!=NULL);
	new->step = step;
	new->next = NULL;
	if (route->end == NULL){
		route->end = route->start = new;
	}
	else{
		route->end->next = new;
		route->end = new;
	}
	return route;
}

void prt_chk_route(route_t *route, int **grid, startend_t startend,
int *status, route_coords *entblock, int print, int *pos, dimension_t dim){
	/* function that checks the route for each of the 5 statuses and also
	prints route, depending on whether the print variable is set to TRUE */

	int row, col, intl_row, intl_col;
	int counter = 0, stepcounter = 0, blocked = 0, start = 0, get_exit = 0;

	intl_row = read_coords(startend.initial, &start);
	intl_col = read_coords(startend.initial, &start);

	node_t *temp, *prev;
	temp = route->start;
	*status = CORRECT_STATUS;

	if (temp == NULL){
		printf("No route input");
		return;
	}
	while (temp != NULL){
		row = temp->step.row;
		col = temp->step.col;

		if (stepcounter == 0){ /* checks if initial cell matches */
			if (!check_intl(row, col, startend)){
				*status = STATUS_1;
			}
		}
		if (temp->next == NULL){
			if (!check_goal(row, col, startend)){ /* checks if goal cell matches */
				*status = STATUS_2;
			}
			if (print){
				printf("[%d,%d].", row, col);
			}
		}
		else{
			if (check_block(row, col, grid) && *status != STATUS_3){
				if (!blocked){
					entblock->row = prev->step.row;
					entblock->col = prev->step.col;
					*pos = stepcounter - 1;
					blocked = get_exit = TRUE;
				}
				*status = STATUS_4;
			}
			else{
				prev = temp;
				if (blocked && get_exit){
					get_exit = FALSE;

				}
				if ((row >= 0 || row <= dim.rows) && (col >= 0 || col <= dim.columns)){
					grid[row][col] = STEP;
				}
			}

			if (check_invalid_move(row, col, temp->next->step.row,
														           temp->next->step.col, dim)){
				*status = STATUS_3;
			}
			if (print){
				printf("[%d,%d]->", row, col);
			}
			if (prev->step.row == temp->next->step.row &&
		  prev->step.col == temp->next->step.col){

				temp = temp->next;
			}
		}

		stepcounter++;
		counter++;
		temp = temp->next;

		if (counter == 5){
			if (print){
				printf("\n");
			}
			counter = 0;
		}
	}
	if (counter != 0 && print){
		printf("\n");
	}
	if (print){
		print_status(*status);
	}
}

int check_intl(int row, int col, startend_t startend){
	/* function checks if there is a mismatch between route start and initial cell */
	int start = 0, row_of_intl, col_of_intl;
	row_of_intl = read_coords(startend.initial, &start);
	col_of_intl = read_coords(startend.initial, &start);
	if (row != row_of_intl && col != col_of_intl){
		return FALSE;
	}
	else{
		return TRUE;
	}
}

int check_goal(int row, int col, startend_t startend){
	/* function checks if there is a mismatch between route end and goal cell */
	int start = 0, row_of_intl, col_of_intl;
	row_of_intl = read_coords(startend.goal, &start);
	col_of_intl = read_coords(startend.goal, &start);
	if (row != row_of_intl && col != col_of_intl){
		return FALSE;
	}
	else{
		return TRUE;
	}
}

int check_invalid_move(int row, int col, int nextrow, int nextcol,
	 											dimension_t dim){
	/* function checks if there is an invalid move being made in the route */
	if (row > dim.rows || col > dim.columns){
		return TRUE;
	}
	if ( (abs(nextrow - row) > 1 ) || (abs(nextcol - col)) > 1 ){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

int check_block(int row, int col, int **grid){
	/* checks if route coords encounter a block */
	if (grid[row][col] == BLOCK){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

void print_status(int status){
	/* function prints the status message according to the value 'status' given */
	if (status == STATUS_1){
		printf("Initial cell in the route is wrong!");
	}
	else if (status == STATUS_2){
		printf("Goal cell in the route is wrong!");
	}
	else if (status == STATUS_3){
		printf("There is an illegal move in this route!");
	}
	else if (status == STATUS_4){
		printf("There is a block on this route!");
	}
	else if (status == NOFIX_STATUS){
		printf("The route cannot be repaired!");
	}
	else{
		printf("The route is valid!");
	}
	printf("\n");
}

void print_grid(int **grid, dimension_t dimension, startend_t startend){
	/* functions prints the grid, as specified in the specifications */

	int row, col, count_intl=0, count_goal=0, row_intl, col_intl, row_goal, col_goal;
	int row_count = 0, col_count = 0;
	row_intl = read_coords(startend.initial, &count_intl);
	col_intl = read_coords(startend.initial, &count_intl);
	row_goal = read_coords(startend.goal, &count_goal);
	col_goal = read_coords(startend.goal, &count_goal);
	for (row = 0; row < dimension.rows + 1; row++){
		for (col = 0; col < dimension.columns + 1; col++){
			if (col == 0 && row == 0){
				printf(" ");
			}
			else if (row != 0 && col == 0){
				if (row_count > GRID_MAX + 1){
					row_count = 1;
				}
				printf("%d", row_count - 1);
			}
			else if (row == 0 && col != 0){
				if (col_count > GRID_MAX + 1){
					col_count = 1;
				}
				printf("%d", col_count - 1);
			}
			else if (row - 1 == row_intl && col - 1 == col_intl){
				printf("I");
			}
			else if (row - 1 == row_goal && col - 1 == col_goal){
				printf("G");
			}
			else if (grid[row - 1][col - 1] == STEP){
				printf("*");
			}
			else if (grid[row - 1][col - 1] == BLOCK){
				printf("#");
			}
			else{
				printf(" ");
			}
			col_count++;
		}
		printf("\n");
		row_count++;
	}
}

int repair_route(int **grid, dimension_t dimension, route_coords *entblock,
								 route_t *route, int *status_msg, int pos){
	/* function constructs an array of pairs, calling it List_pair and start
	forming pairs starting from where the block occurs, given by 'entblock' */

	pair *list_pair, newpair;
	int numdistinct=0, queue_count = 0, get_pair_num;
	newpair.repair_step = *entblock;
	newpair.numstep = 0;
	list_pair = (pair*)malloc(dimension.rows*dimension.columns*sizeof(newpair));
	list_pair[numdistinct] = newpair;
	numdistinct++;
	while ((get_pair_num = (get_pair(grid, dimension, *entblock, queue_count,
				 list_pair, &numdistinct, route))) == TRUE){
		queue_count++;
	}

	if (in_route(list_pair[numdistinct - 1].repair_step.row,
	list_pair[numdistinct - 1].repair_step.col, route, *entblock)){
		/* if the specified coordinate in the list of pairs fall in the route */

		get_repaired_route(grid, list_pair, numdistinct - 1, route, pos);
		*status_msg = STATUS_5;
		if (get_pair_num == FALSE){
			return STOP;
		}
	}
	else{
		*status_msg = NOFIX_STATUS;
	}
	return 0;
}

int get_pair(int **grid, dimension_t dimension, route_coords entblock,
						int queue_count, pair *list_pair, int *numdistinct, route_t *route){
	 /* function gets all possible pairs of steps until no possible pairs can be
	 found, or a step is found that falls within the route, hence repaired */

	 int uprow, upcol, downrow, downcol, leftrow, leftcol, rightrow, rightcol;
	 int counter = list_pair[queue_count].numstep, num = *numdistinct;
	 int move_up, move_down, move_left, move_right;
	 uprow = list_pair[queue_count].repair_step.row - 1;
	 upcol = list_pair[queue_count].repair_step.col;
	 move_up = check_move(list_pair, queue_count, grid,
		 										uprow, upcol, dimension, entblock, num, route);
	 if (move_up){
		 insert_pair(list_pair, num, uprow, upcol, counter);
		 num++;
		 *numdistinct = num;
	 }
	 if (move_up == STOP){ /* the STOP value indicates that a repair is found */
		 return STOP;
	 }
	 num = *numdistinct;

	 downrow = list_pair[queue_count].repair_step.row + 1;
	 downcol = list_pair[queue_count].repair_step.col;
	 move_down = check_move(list_pair, queue_count, grid,
		 									  	downrow, downcol, dimension, entblock, num, route);
	 if (move_down){
		 insert_pair(list_pair, num, downrow, downcol, counter);
		 num++;
		 *numdistinct = num;
 	 }
 	 if (move_down == STOP){
 		 return STOP;
 	 }
	 num = *numdistinct;

	 leftrow = list_pair[queue_count].repair_step.row;
	 leftcol = list_pair[queue_count].repair_step.col - 1;
	 move_left = check_move(list_pair, queue_count, grid,
		  										leftrow, leftcol, dimension, entblock, num, route);
	 if (move_left){
	 	 insert_pair(list_pair, num, leftrow, leftcol, counter);
		 num++;
		 *numdistinct = num;
 	 }
 	 if (move_left == STOP){
 	 	 return STOP;
 	 }
	 num = *numdistinct;

	 rightrow = list_pair[queue_count].repair_step.row;
	 rightcol = list_pair[queue_count].repair_step.col + 1;
	 move_right = check_move(list_pair, queue_count, grid,
		 											 rightrow, rightcol, dimension, entblock, num, route);
	 if (move_right){
	   insert_pair(list_pair, num, rightrow, rightcol, counter);
		 num++;
		 *numdistinct = num;
		}
	 if (move_right == STOP){
		 return STOP;
		}
	 num = *numdistinct;

	 if (!move_up && !move_down && !move_left && !move_right && num==queue_count){
	 	 return FALSE;
	 }
	 return TRUE;   /* return true allows the while loop of pairs to be active */
}

void insert_pair(pair *list_pair, int num, int row, int col, int counter){
	/* this function inserts the pairs into our list of pairs */

	route_coords direction;
	direction.row = row; direction.col = col;
	list_pair[num].repair_step = direction;
	list_pair[num].numstep = counter + 1;
}

int check_if_in_queue(pair *list_pair, int row, int col, int num){
	/*function checks if found pair is in our list, and if so then ignores pair*/
	int counter = 0;
	for (counter = 0; counter < num; counter++){
		if (list_pair[counter].repair_step.row == row &&
		list_pair[counter].repair_step.col == col){
			return TRUE;
		}
	}
	return FALSE;
}

int check_move(pair *list_pair, int count, int **grid, int row, int col,
dimension_t dimension, route_coords entblock, int num, route_t *route){
	/* checks if a move made when forming a pair is valid */

	if (row >= dimension.rows || col >= dimension.columns ||
	row < 0 || col < 0){
		return FALSE;
	}
	else if (grid[row][col] != BLOCK && in_route(row, col, route, entblock)){
		return STOP;
	}
	else if (grid[row][col]!= BLOCK && !check_if_in_queue(list_pair, row, col, num)){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

int in_route(int row, int col, route_t *route, route_coords entblock){
	/* function checks if generated pair falls within the route */

	node_t *temp, *temp2;
	int counter = 0, step_count = 0;
	temp = temp2 = route->start;
	while (temp2!=NULL){
		if (temp2->step.row == entblock.row && temp2->step.col == entblock.col){
			counter++;
		}
		temp2 = temp2->next;
	}

	while (step_count != counter){
		if (temp->step.row == entblock.row && temp->step.col == entblock.col){
			step_count++;
		}
		temp = temp->next;
	}
	while (temp!=NULL){

		if (temp->step.row == row && temp->step.col == col){
			return TRUE;
		}
		temp = temp->next;
	}
	return FALSE;
}

void get_repaired_route(int **grid, pair *list_pair, int num, route_t *route,
int pos){ /* this function forms the repaired route fragment and inserts it*/

	int counter = 0, next_down = TRUE, next_left = TRUE, next_right = TRUE;
	route_coords start_point, end_point;
	node_t *temp, *end_node;
	start_point.row = list_pair[num].repair_step.row;
	start_point.col = list_pair[num].repair_step.col;
	end_point.row = list_pair[0].repair_step.row;
	end_point.col = list_pair[0].repair_step.col;
	int startstep = list_pair[num].numstep;
	temp = route->start;
  temp = restart_route(temp, end_point, start_point, FALSE, route, pos);
	end_node = route->start;
  end_node = restart_route(end_node, start_point, end_point, TRUE, route, pos);
	temp->next = end_node;

	while (startstep > 1){
		temp = restart_route(temp, end_point, start_point, FALSE, route, pos);

		for (counter = num - 1; counter > 0; counter--){ /* move up */
			if (move_step(list_pair, counter, start_point.row - 1,
																				start_point.col, startstep)){
				grid[start_point.row - 1][start_point.col] = STEP;
				start_point.row--;
				insert_repair(route, temp, start_point);
				startstep--;
				next_down = next_left = next_right = FALSE;
				break;
			}
		}

		if (next_down){
			for (counter = num - 1; counter > 0; counter--){ /* move down */
				if (move_step(list_pair, counter, start_point.row + 1,
					 																start_point.col, startstep)){
					grid[start_point.row + 1][start_point.col] = STEP;
					start_point.row++;
					insert_repair(route, temp, start_point);
					startstep--;
					next_left = next_right = FALSE;
					break;
				}
			}
		}
		if (next_left){
			for (counter = num - 1; counter > 0; counter--){   /* move left */
				if (move_step(list_pair, counter, start_point.row,
					 																start_point.col - 1, startstep)){
					grid[start_point.row][start_point.col - 1] = STEP;
					start_point.col --;
					insert_repair(route, temp, start_point);
					startstep --;
					next_right = FALSE;
					break;
				}
			}
		}
		if (next_right){
			for (counter = num - 1; counter > 0; counter--){   /* move right */
				if (move_step(list_pair, counter, start_point.row,
					 																start_point.col + 1, startstep)){
					grid[start_point.row][start_point.col + 1] = STEP;
					start_point.col++;
					insert_repair(route, temp, start_point);
					startstep --;
					break;
				}
			}
		}
		next_down = next_left = next_right = TRUE;
	}
}

node_t *restart_route(node_t *temp, route_coords start_point,
route_coords end_point, int get_end_node, route_t *route, int pos){
	/* function resets the pointer temp to the entry block position, to insert
																												the repaired route */
	node_t *temp2;
	int counter=0;
	temp2 = route->start;
	temp = route->start;
	if (get_end_node){
		while (temp->step.row != end_point.row || temp->step.col != end_point.col){
			temp = temp->next;
		}
		while (temp!=NULL){
			if (temp->step.row == start_point.row && temp->step.col == start_point.col){
				return temp;
			}
			temp = temp->next;
		}
	}
	while(counter != pos){
		counter++;
		temp = temp->next;
	}
	return temp;
}

void insert_repair(route_t *route, node_t *temp, route_coords repair){
	/* function inserts a repaired step into the incorrect route */
	node_t *new;
	new = (node_t *)malloc(sizeof(*new));
	assert(route!=NULL && new!=NULL);
	new->step = repair;
	new->next = temp->next;
	temp->next = new;
}

int move_step(pair *list_pair, int num, int row, int col , int startstep){
	/* function checks if element in the pair list forms the repaired route */
	if (row == list_pair[num].repair_step.row &&
	col == list_pair[num].repair_step.col &&
	startstep - 1 == list_pair[num].numstep){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

/* Algorithms are fun! */
