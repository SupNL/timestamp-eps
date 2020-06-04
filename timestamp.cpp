#include <iostream>
#include <windows.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <conio.h>
#include <string>
using namespace std;

string getKeyName(UINT code){
    switch(code){
        case VK_SPACE:
            return "ESPACO";
        case MOD_ALT:
            return "ALT";
    }
}

string replaceSpace(string text){
    replace(text.begin(), text.end(), ' ', '_');
    return text;
}

void aguardarInicio(){
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0 && msg.wParam != 1){}
}

void mensagemRegistro(){
    system("cls");
    cout << "Iniciou!\n";
    cout << "Aperte CTRL + ALT + S para registrar o tempo\n";
    cout << "Aperte CTRL + ALT + A caso tenha ocorrido um erro no seu episodio\n";
    cout << "Aperte CTRL + ALT + ESPACO para finalizar\n\n";
}

int converterParaSegundos(string tempo){
    stringstream ss(tempo);
    string item;

    int hora, minuto, segundo;
    getline(ss, item, ':');
    hora = stoi(item);
    getline(ss, item, ':');
    minuto = stoi(item);
    getline(ss, item, ':');
    segundo = stoi(item);

    return (hora * 3600) + (minuto * 60) + segundo;
}

string converterParaTempoTextual(int segundo){
    string tempo;
    int hora, minuto;
    if(segundo < 0)
        segundo = 0;
    hora = (int) segundo / 3600;
    segundo = segundo % 3600;
    minuto = (int) segundo / 60;
    segundo = segundo % 60;

    string horaTexto, minutoTexto, segundoTexto;
    if(hora < 10)
        horaTexto = "0" + to_string(hora);
    else
        horaTexto = to_string(hora);
    if(minuto < 10)
        minutoTexto = "0" + to_string(minuto);
    else
        minutoTexto = to_string(minuto);
    if(segundo < 10)
        segundoTexto = "0" + to_string(segundo);
    else
        segundoTexto = to_string(segundo);

    tempo = horaTexto + ':' + minutoTexto + ':' + segundoTexto;
    return tempo;
}

void inicializarRegistro(ofstream* arquivo){
    MSG msg = {0};
    time_t inicio, marcar, offset;
    string tempo;
    offset = 0;
    inicio = time(NULL);

    if(
        RegisterHotKey(NULL, 2, MOD_ALT | MOD_CONTROL | 0x4000, 0x53) && /// tecla S
        RegisterHotKey(NULL, 3, MOD_ALT | MOD_CONTROL | 0x4000, 0x41) /// tecla A
    ){
        mensagemRegistro();
        while (GetMessage(&msg, NULL, 0, 0) != 0 && msg.wParam != 1){
            if (msg.wParam == 2){
                marcar = time(NULL);
                tempo = converterParaTempoTextual(marcar - inicio - 1 + offset);
                *arquivo << tempo << '\n';
                cout << "Registrado em " << tempo << '\n';
            }else if (msg.wParam == 3){
                cout << "Parece que ocorreu um erro!\n";
                cout << "Me informe onde voce vai comecar (formato hh:mm:ss): ";
                cin >> tempo;
                offset = converterParaSegundos(tempo);
                system("cls");
                cout << "Pronto para retomar a partir de " << tempo << '\n';
                cout << "Aperte CTRL + ALT + ESPACO quando retomar o anime\n";
                aguardarInicio();
                inicio = time(NULL);
                mensagemRegistro();
            }
        }
    }else{
        cout << "Um erro inesperado ocorreu\n";
    }
    UnregisterHotKey(NULL, 2);
    UnregisterHotKey(NULL, 3);
}

void prepararRegistro(){
    MSG msg = {0};
    system("cls");
    string nomeAnime, seasonAnime, epAnime;
    string nomeArquivo;

    if(RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | 0x4000, VK_SPACE)){
        cout << "Antes de tudo, para registrar adequadamente, nao feche o programa antes de finalizar\n";
        cout << "Agora me informe certinho:\n";
        cout << "O nome do anime (sem espacos!): ";
        cin >> nomeAnime;
        nomeAnime = replaceSpace(nomeAnime);
        cout << "A temporada: ";
        cin >> seasonAnime;
        seasonAnime = replaceSpace(seasonAnime);
        cout << "O episodio: ";
        cin >> epAnime;
        epAnime = replaceSpace(epAnime);

        nomeArquivo = nomeAnime + "_temp" + seasonAnime + "_ep" + epAnime;
        ofstream arquivo;
        arquivo.open(nomeArquivo + ".txt");

        /// Aguardar input do teclado
        system("cls");
        cout << "Eu estou pronto para comecar!\n";
        cout << "Aperte CTRL + ALT + ESPACO quando comecar a assistir\n";
        cout << "Nao se esqueca, eu rodo no fundo!\n";
        aguardarInicio();

        /// Ao apertar
        inicializarRegistro(&arquivo);

        /// Finalizacao
        arquivo.close();
        system("cls");
        cout << "Concluido! Salvo no arquivo " << nomeArquivo << ".txt" << '\n';
        cout << "Pressione ENTER para finalizar\n";
        getch();
    }else{
        DWORD errorMessageID = ::GetLastError();

        cout << "Houve algum erro ao inicializar!\n";
        cout << "Aperte enter para retomar ao menu\n";
        cout << "Codigo de erro: " + errorMessageID;
        
        getch();
    }

    UnregisterHotKey(NULL, 1);
}

void mensagemMenu(){
    cout << "Bem-vindo! Vamos marcar o tempo?\n";
    cout << "0. Sair\n";
    cout << "1. Comecar a gravar\n";
    cout << "Opcao: ";
}

int main(){
    int opcao;
    do{
        system("cls");
        mensagemMenu();
        cin >> opcao;
        if(opcao == 1)
            prepararRegistro();
    }while(opcao != 0);
    return 0;
}
