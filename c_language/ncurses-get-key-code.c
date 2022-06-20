#include <ncurses.h>

int main()
{
    initscr();
    keypad(stdscr, true);   //Ative o modo de leitura das teclas de função
    noecho();               //Desligue a exibição de caracteres de entrada, necessários para getch()
    halfdelay(100);         //Defina o limite de tempo limite getch() para 10 segundos

    printw("Press F2 to exit.\n");

    bool ex = false;
    while ( !ex )
    {
        int ch = getch();

        switch ( ch )
        {
        case ERR:
            printw("Please, press any key...\n"); //Se não houve pressionamento, lembramos o usuário de pressionar uma tecla
            break;
        case KEY_F(2): //Sair do programa se F2 foi pressionado
            ex = true;
            break;
        default:  //Se tudo estiver bem, exibimos o código da tecla pressionada
            printw("Code of pressed key is %d\n", ch);
            break;
        }

        refresh(); //Trazendo para a tela real

    }

    printw("Thank you. Good buy!");
    getch();
    endwin(); 
    return 0;
}