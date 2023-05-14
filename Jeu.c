/*
  Projet programmation 1 Pam
  ==============================================
  Prénom        : Redwan
  Nom           : Khan
  N° Etudiant   : 12212140
  ==============================================
  SQUARE.C

  Je déclare qu'il s'agit de mon propre travail.
  ==============================================

*/

#include <ncurses.h>      // Pour créer une interface textuelle avec les fonctions interactives 
#include <stdlib.h>      //  rand()
#include <time.h>       //   srand(time(NULL))  et les fonctions pour faire le chrono
#include <unistd.h>    //    usleep()
#include <string.h>   //     strlen()



// Taille de la fenetre ncurses
#define HAUTEUR 30
#define LARGEUR 90

// Taille de l'objet qui sera controlé
#define TailleSquare_Y 2
#define TailleSquare_X 4


// Pour avoir les positions des objets : 
struct Position{
  int y;
  int x;
};

// Pour simplifier les tâches :
struct Position mwin; // Coordonnées de la fenêtre principale

struct Position square; // Coordonnées du carré 

struct Position food; // Coordonnées de la nourriture



/***** Menu du jeu *****/

// Affiche lettre par lettre le message pour faire un effet animation
void animation(char tab[], int ligne);


// Cette fonction affiche le menu et renvoie l'option choisie par l'utilisateur.
int menu();

// Affichage des choix à l'utilisateur et renvoie celui sélectionné à menu().
int selectChoice(int x, int y);

// Cette fonction affiche des messages d'aide à l'utilisateur.
void affiche_aides();

int quit; // Pour indiquer si le joueur veut quitter le jeu ou non.

/*****           *****/


/* Fonction principale :
 * Cette fonction gère l'affichage du Jeu,
 * L'interaction,
 * La logique,
 * Et la progression d'une partie de Square*/
int jouer();


/* La fonction deplacement permet de déplacer le carré
 * Sur l'écran selon la touche appuyée en paramètre.*/
void deplacement(int ch);


/* La fonction collision verifie si la position du carré et la position de la nourriture sont les mêmes.
 * Elle renvoie 1 si le carré est en collision avec la nourriture, sinon elle renvoie 0.*/
int collision_food();


/* Cette fonction calcule le temps écoulé depuis le début du chronomètre. */
double chrono_secs(clock_t start, clock_t end);


/* Demande au joueur s'il veut rejouer, si oui, renvoie 1 sinon 0 */
int rejouer();


int main(){
  initscr();                   // Initialisation de ncurses

  cbreak();                  // Désactiver le mode canonique
  noecho();                 // Désactiver l'affichage des caractères saisis
  keypad(stdscr, TRUE);    // Active les touches du claviers
  nodelay(stdscr, TRUE);  // Lecture non bloquante
  curs_set(FALSE);       // Désactive le curseur

  srand(time(NULL));   // Initialise le générateur de nombres aléatoires pour la nourriture


  /* Message de Bienvenue */
  char title[] = "Bienvenue !";
  char start[] = "Appuyez sur une touche pour commencer le jeu...";

  // Affiche la phrase lettre par lettre, pour avoir l'effet animation
  animation(title, 2);
  animation(start, 0);
  

  getchar(); // Attend que le joueur appuie sur une touche 
  clear(); // Efface l'écran

  while (COLS < LARGEUR || LINES < HAUTEUR){
    char attention[] = "Merci de bien vouloir aggrandir la fenetre du terminal...";
    mvprintw(LINES / 2, (COLS - strlen(attention)) / 2, "%s", attention);

    getch();
    clear();
  }

  int continuer = 1; // Booléen pour savoir si on continue le jeu ou pas 

  int score = 0;          // Score du joueur 
  int meilleur_score = 0; // Meilleur score atteint par le joueur 
  int compteur = 0;       // Pour donner le meilleur score qu'après la 2e lancement du jeu

  while (continuer){
    clear(); // Efface tous ce qu'il avait avant 

     
    menu(); // Affiche le menu du jeu

    if (quit == EXIT_FAILURE){
      quit = EXIT_SUCCESS;
      break;
    }
    

    clear(); 

    // Parametrage de la fênetre principale centré dans le terminal
    mwin.y = (LINES - HAUTEUR) / 2;
    mwin.x = (COLS - LARGEUR) / 2;

    // Parametrage du carré pour qu'il s'affiche au milieu de la fenetre
    square.y = (HAUTEUR - TailleSquare_Y) / 2;
    square.x = (LARGEUR - TailleSquare_X) / 2;

    /* Parametrage de la nourriture pour qu'il s'affiche
     * aléatoirement dans la fenetre sans touche les bordures */
    food.y = rand() % (HAUTEUR - 2) + 1;
    food.x = rand() % (LARGEUR - 3) + 1;

    
    score = jouer(); // Lance le jeu et récupère le score du joueur 
    compteur++;

    printf("\nDommage ! Votre score est : %d\n", score);

    if (compteur == 1)
      meilleur_score = score;  // Si nouveau record 
    
    else if (score > meilleur_score && compteur > 1){
      meilleur_score = score;
      printf("\nFélicitations, vous avez battu votre record !\n");
    }

    //Demande si le joueur veut rejouer ou pas, si oui, recommence le jeu sinon sort de la boucle.
    continuer = rejouer();

    if (continuer)
      score = 0;

  }

  endwin();
  printf("\nMerci d'avoir jouer, à bientôt !\n\n");

  return 0;
}



/* Fonction principale :
 * Crée et affiche la fenêtre du jeu.
 * Active les touches du clavier pour interagir avec le jeu.
 * Initialise les couleurs.
 * Effectue la gestion du temps et du score pendant la partie.
 */
int jouer(){

  clear(); // Efface tous ce qu'il avait avant 

  int score = 0; // Initialise le score  

  // Déclarations des variables pour afficher le chrono 
  double secondes;
  int millisecs;
  clock_t start = clock();

  // Création et affichage de notre fenetre de jeu avec les bordures 
  WINDOW *mainwin = newwin(HAUTEUR, LARGEUR, mwin.y, mwin.x);
  box(mainwin, 0, 0);

  keypad(mainwin, TRUE);  // Activation des touches du clavier à l'interieur dans la fenetre 
  nodelay(mainwin, TRUE); // Pour déplacer le carré en temps réel 

  // Parametrage des couleurs
  start_color();
  


  init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Couleur Jaune pour la nourriture 


    // Boucle principale du jeu 
    while (TRUE){
      werase(mainwin);    // Efface la fenêtre du jeu 

      box(mainwin, 0, 0);

      mvwprintw(mainwin, 0, 2, " Score : %d ", score); // Affiche le score 
      mvwprintw(mainwin, 0, 20, " Temps restant : %d.%02ds ", (int)secondes, millisecs); // Affiche le chrono 

      // Active le mode inverse pour afficher les objets de notre jeu 
      wattron(mainwin, A_REVERSE);
      
      // Affiche la nourriture 
      wattron(mainwin, COLOR_PAIR(1));
      mvwprintw(mainwin, food.y, food.x, "  ");
      wattroff(mainwin, COLOR_PAIR(1));

      // Affiche le carré 
      for (int i = 0; i < TailleSquare_Y; i++)
      {
        for (int j = 0; j < TailleSquare_X; j++)
        {
          mvwprintw(mainwin, square.y + i, square.x + j, " ");
        }
      }
    wattroff(mainwin, A_REVERSE);

    wrefresh(mainwin); // Rafraîchit la fenêtre pour avoir les nouveaux affichages de la fenetre 

    int ch = wgetch(mainwin); // Attend une touche du clavier 

    /* Plus envis de jouer ? Appuyer simplement sur q :
     * Supprime la fenêtre pour libérer le stokage utilisé
     * Ferme ncureses
     * Retourne le score du joueur
     */
    if (ch == 'q'){
      delwin(mainwin);
      endwin();
      return score;
    }

    deplacement(ch); // Déplacement du carré 

    /* Si le carré mange la nourriture dans le temps :
     * Augmente le score.
     * Changer la position de la nourriture.
     * Réinitialise le temps.
     */
    if (collision_food()){
      score++;
      food.y = rand() % (HAUTEUR - 2) + 1;
      food.x = rand() % (LARGEUR - 3) + 1;
      start = clock();
    }

    /***** Calcule le temps restant *****/

    clock_t end = clock();

    secondes = chrono_secs(start, end) - (0.2 * score); // Mais diminue en fonction du score du joueur 

    /* Calcule les milliesecondes pour donner un peu de piment au joueur
     * Prend que la partie décimale des secondes.
     * Par exemple, si secondes = 3,75 alors millisecs = 75 */
    millisecs = ((secondes - (int)secondes) * 100);


    /* Si le joueur n'arrive pas manger la nourriture à temps :
     * Arrete le jeu et revoie le score */
    if (secondes <= 0){
      delwin(mainwin);
      endwin();
      return score;
    }
  }
}


/* Fonction qui prend en paramétre la touche saisie par le joueur :
 * Pour  déplacer le carré dans la direction selon de la touche directionelle.
 * Vérifie quelle touche le joueur a entré.
 * Vérifie si le carré n'est pas au bord de la fenetre.
 * Augmente ou diminue à la postion du carré selon la touche.
 */
void deplacement(int ch)
{
  if (ch == KEY_UP)
  { // Si la touche est ↑
    if (square.y > 1)
    { // Si le carré n'est pas déjà tout en haut de l'écran
      square.y--;
    }
  }
  else if (ch == KEY_DOWN)
  { // Si la touche est ↓
    if (square.y < HAUTEUR - TailleSquare_Y - 1)
    { // Si le carré n'est pas déjà tout en bas de l'écran
      square.y++;
    }
  }
  else if (ch == KEY_LEFT)
  { // Si la touche est ←
    if (square.x > 1)
    { // Si le carré n'est pas déjà tout à gauche de l'écran
      square.x--;
    }
  }
  else if (ch == KEY_RIGHT)
  { // Si la touche est →
    if (square.x < LARGEUR - TailleSquare_X - 1)
    { // Si le carré n'est pas déjà tout à droite de l'écran
      square.x++;
    }
  }
}


/* Fonction qui vérifie si la position de 2 objets se chevauchent :
 * Retourne 1 pour indiquer qu'il y a collision.
 * Sinon, elle retourne 0.
 */
int collision_food(){
  if (square.y <= food.y && food.y < square.y + TailleSquare_Y &&
      square.x <= food.x && food.x < square.x + TailleSquare_X){
    return 1; // Si le carré est en collision avec la nourriture
  }

  return 0; // Si le cas contraire.
}


/* Cette fonction calcule le temps écoulé en secondes
 * depuis (start) jusqu'à (end).
 * Elle retourne le temps restant des 10 secs */
double chrono_secs(clock_t start, clock_t end){
  // Calcul de temps écoulé.
  double temps_ecouler = (double)(end - start) / CLOCKS_PER_SEC;

  // Le chrono commence de 10s par défaut 
  double secondes = 10.0 - temps_ecouler;

  return secondes;
}



// Demande au joueur s'il veut rejouer
int rejouer(){
    char rejouer;

    printf("\nVoulez-vous rejouer ? (O/N) ");
    scanf("%c", &rejouer);
    getchar(); // Pour être sûr car parfois, ça ne demande pas...

    if (rejouer == 'o' || rejouer == 'O') {
        return 1;
    }
    
    return 0;
  }


void animation(char tab[], int ligne){
  int len = strlen(tab);
  for (int i = 0; i < len; i++) {
    mvprintw(LINES / 2 - ligne, (COLS - len) / 2 + i, "%c", tab[i]);
    refresh();     // Rafraîchit l'écran pour afficher la lettre
    usleep(30000); // Pause de 30 millisecondes
  }
}


/*Affichage de menu */
int menu(){
  attron(A_BOLD); // Pour donner un effet gras car les textes du terminal sont petits et minces.

  // Affiche LOGO, fait sur patorjk.com
  char title1[] = " _______  _______           _______  _______  _______ ";
  char title2[] = "(  ____ \\(  ___  )|\\     /|(  ___  )(  ____ )(  ____ \\";
  char title3[] = "| (    \\/| (   ) || )   ( || (   ) || (    )|| (    \\/";
  char title4[] = "| (_____ | |   | || |   | || (___) || (____)|| (__    ";
  char title5[] = "(_____  )| |   | || |   | ||  ___  ||     __)|  __)   ";
  char title6[] = "      ) || | /\\| || |   | || (   ) || (\\ (   | (      ";
  char title7[] = "/\\____) || (_\\ \\ || (___) || )   ( || ) \\ \\__| (____/\\";
  char title8[] = "\\_______)(____\\/_)(_______)|/     \\||/   \\__/(_______/";

  char indication[] = "Sélectionner puis appuyer sur ENTREE pour valider.";

  // Affiche LOGO au centre du terminal
  mvprintw(LINES / 2 - 10, (COLS - strlen(title1)) / 2, "%s", title1);
  mvprintw(LINES / 2 - 9, (COLS - strlen(title2)) / 2, "%s", title2);
  mvprintw(LINES / 2 - 8, (COLS - strlen(title3)) / 2, "%s", title3);
  mvprintw(LINES / 2 - 7, (COLS - strlen(title4)) / 2, "%s", title4);
  mvprintw(LINES / 2 - 6, (COLS - strlen(title5)) / 2, "%s", title5);
  mvprintw(LINES / 2 - 5, (COLS - strlen(title6)) / 2, "%s", title6);
  mvprintw(LINES / 2 - 4, (COLS - strlen(title7)) / 2, "%s", title7);
  mvprintw(LINES / 2 - 3, (COLS - strlen(title8)) / 2, "%s", title8);

  // Affiche le message juste en dessous du LOGO
  mvprintw(LINES / 2 - 1, (COLS - strlen(indication)) / 2, "%s", indication);

  // Affiche les choix du menu
  char choix1[] = " - Jouer - ";
  char choix2[] = " - Aides - ";
  char choix3[] = " - Crédit - ";
  char choix4[] = " - Quitter - ";

  mvprintw(LINES / 2 + 4, (COLS - 12) / 2, "%s", choix1);
  mvprintw(LINES / 2 + 6, (COLS - 12) / 2, "%s", choix2);
  mvprintw(LINES / 2 + 8, (COLS - 14) / 2, "%s", choix3);
  mvprintw(LINES / 2 + 10, (COLS - 14) / 2, "%s", choix4);

  /***** Le joueur doit sélectionner un choix *****/

  int x = LINES / 2 + 4;
  int y = COLS / 2;

  getch(); // Inutile mais cool pour le visuel

  // Affiche des ">>" et "<<" à chaque coté du choix sélectionner par le joueur
  int choix = selectChoice(x, y);

  clear(); // Efface tous ce qui avait été print

  // Si choix "Jouer"
  if (choix == 1) // On commmence directement le jeu
    return EXIT_SUCCESS;

  // Si choix "Aides"
  else if (choix == 2){ // Affiage un message pour aider à guider le joueur
    affiche_aides();
    refresh();   
  }

  // Si choix "Crédit"
  else if (choix == 3){ // Encore une chose futile...
    mvprintw((LINES) / 2, (COLS - 35) / 2, "* Copyright(C) 2023, Redwan Khan. ");

    attron(A_STANDOUT);
    mvprintw((LINES) / 2 + 2, (COLS - 22) / 2, " Meilleur Score : 38 ");
    attroff(A_STANDOUT);
    getch();
    refresh(); // Ajoutez cette ligne pour rafraîchir l'écran
  }

  else if (choix == 4){
    quit = EXIT_FAILURE;
    return quit;
  }

  attroff(A_BOLD); // Trop de texte gras, ça rend illisible

    getchar();
    clear();
    menu();


  return EXIT_SUCCESS;
}

/* Cette fonction affiche des flèches indiquant les choix disponibles à l'utilisateur.
 * Elle lit les entrées clavier de l'utilisateur
 * Et met à jour l'affichage pour refléter le choix actuellement sélectionné, en déplaçant les flèches.
 * Retourne le choix finalement sélectionné par l'utilisateur. */
int selectChoice(int x, int y) {
  int ch;
  mvprintw(x, y - 11, ">>");
  mvprintw(x, y + 6, "  <<");

  // Boucle tant que le joueur n'a pas valider un choix
  int choix = 1;
  while ((ch = getch()) != '\n') {
    if (ch == KEY_UP) {
      if (choix > 1) {
        mvprintw(x, y - 11, "    ");
        mvprintw(x, y + 6, "    ");
        x -= 2;
        choix--;
      }
    }
    else if (ch == KEY_DOWN) {
      if (choix < 4) {
        mvprintw(x, y - 11, "    ");
        mvprintw(x, y + 6, "    ");
        x += 2;
        choix++;
      }
    }

    mvprintw(x, y - 11, ">>");
    mvprintw(x, y + 6, "  <<");
  }

  return choix;
}

/* Cette fonction qui affiche un cadre rectangulaire au centre de l'écran
 * Contenant des messages d'aide pour le jeu. */
void affiche_aides() {
  // Calculs des coordonnées des bordures à prints
  int rect_height = 19;
  int rect_width = 80;

  int rect_y = (LINES - rect_height) / 2;
  int rect_x = (COLS - rect_width) / 2;

  // Affichage des bordures
  for (int i = rect_y; i < rect_y + rect_height; i++) {
    for (int j = rect_x; j < rect_x + rect_width; j++) {
      if (i == rect_y || i == rect_y + rect_height - 1 ||
        j == rect_x || j == rect_x + rect_width - 1) {
        mvprintw(i, j, "*");
      }
    }
  }

  // Affichage du message d'aide
  mvprintw(rect_y, rect_x + (rect_width - 32) / 2,
           "|           AIDE            |");

  attron(A_STANDOUT);
  mvprintw(rect_y + 2, rect_x + (rect_width - 15) / 2, " BUT DU JEU : ");
  attroff(A_STANDOUT);

  mvprintw(rect_y + 4, rect_x + (rect_width - 31) / 2,
           "Un carré blanc dans l'arène.");
  mvprintw(rect_y + 5, rect_x + (rect_width - 40) / 2,
           "Cherche à manger, toujours en sereine.");
  mvprintw(rect_y + 6, rect_x + (rect_width - 44) / 2,
           "Dirige-le bien, avec les flèches au doigt,");
  mvprintw(rect_y + 7, rect_x + (rect_width - 37) / 2,
           "Plus il mange, plus le score croît,");

  mvprintw(rect_y + 9, rect_x + (rect_width - 33) / 2,
           "Mais gare aux dangers en avant !");
  mvprintw(rect_y + 10, rect_x + (rect_width - 40) / 2,
           "La vitesse grandit, un défi permanent,");
  mvprintw(rect_y + 11, rect_x + (rect_width - 47) / 2,
           "Chaque seconde compte, pas de répit apparent.");

  mvprintw(rect_y + 13, rect_x + (rect_width - 45) / 2,
           "Surveille le chrono, pour ne pas dérailler,");
  mvprintw(rect_y + 14, rect_x + (rect_width - 39) / 2,
           "En un clin d'oeil, tout peut chavirer.");

  attron(A_STANDOUT);
  mvprintw(rect_y + 16, rect_x + (rect_width - 43) / 2,
           " Que La Réussite Soit Avec Toi, Joueur ! ");
  attroff(A_STANDOUT);


    refresh(); 
}
