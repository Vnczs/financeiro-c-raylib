#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "dados_graficos.txt"


typedef struct {
    float valor;
    char tipo;
} Transacao;

const char* FormatarDinheiro(float valor) {
    static char buffer[64];
    char temp[64];
    int n = 0, i, j, k, cont, posFim, pontos, tamFinal;
    
    sprintf(temp, "%.2f", valor); 
    
    while (temp[n] != '.') n++; // Localiza o ponto decimal
    
    posFim = n;
    pontos = (posFim - 1) / 3;
    tamFinal = posFim + pontos + 3; 
    
    buffer[tamFinal] = '\0';
    buffer[tamFinal - 1] = temp[n + 2];
    buffer[tamFinal - 2] = temp[n + 1];
    buffer[tamFinal - 3] = ','; 
    
    k = tamFinal - 4;
    cont = 0;
    
    
    for (i = posFim - 1; i >= 0; i--) {
        if (cont == 3) {
            buffer[k--] = '.';
            cont = 0;
        }
        buffer[k--] = temp[i];
        cont++;
    }
    return buffer;
}

int main() {
   
    InitWindow(600, 750, "Financeiro Pro - Padrao Bancario");
    SetTargetFPS(60);

   
    Font fontePrincipal = LoadFontEx("C:/Windows/Fonts/arial.ttf", 32, 0, 250); 
    SetTextureFilter(fontePrincipal.texture, TEXTURE_FILTER_BILINEAR);

    float saldo = 0.0f;
    char textoEntrada[20] = "\0";
    int contLetras = 0;
    bool caixaAtiva = false;
    int i, totalTransacoes = 0;
    Transacao *historico = NULL;

    FILE *f = fopen(ARQUIVO, "r");
    if (f) { 
        if (fscanf(f, "%f %d", &saldo, &totalTransacoes) != EOF) {
            if (totalTransacoes > 0) {
                historico = (Transacao *)malloc(totalTransacoes * sizeof(Transacao));
                for (i = 0; i < totalTransacoes; i++) {
                    fscanf(f, "%f %c", &historico[i].valor, &historico[i].tipo);
                }
            }
        }
        fclose(f);
    }

    Rectangle caixaTexto = { 150, 210, 300, 45 };
    Rectangle btnGanho = { 80, 280, 210, 50 };
    Rectangle btnGasto = { 310, 280, 210, 50 };

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        
        if (CheckCollisionPointRec(mouse, caixaTexto)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) caixaAtiva = true;
        } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            caixaAtiva = false;
        }

        if (caixaAtiva) {
            int tecla = GetCharPressed();
            while (tecla > 0) {
                if (((tecla >= '0' && tecla <= '9') || tecla == '.') && contLetras < 18) {
                    textoEntrada[contLetras++] = (char)tecla;
                    textoEntrada[contLetras] = '\0';
                }
                tecla = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && contLetras > 0) {
                textoEntrada[--contLetras] = '\0';
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            float valorDigitado = (float)atof(textoEntrada);
            char acao = ' ';

            if (CheckCollisionPointRec(mouse, btnGanho)) {
                saldo += valorDigitado;
                acao = 'G';
            } else if (CheckCollisionPointRec(mouse, btnGasto)) {
                saldo -= valorDigitado;
                acao = 'D';
            }

            if (acao != ' ' && valorDigitado > 0) {
                historico = (Transacao *)realloc(historico, (totalTransacoes + 1) * sizeof(Transacao));
                historico[totalTransacoes].valor = valorDigitado;
                historico[totalTransacoes].tipo = acao;
                totalTransacoes++;
                textoEntrada[0] = '\0';
                contLetras = 0;
            }
        }

        BeginDrawing();
            ClearBackground((Color){ 240, 242, 245, 255 }); 

            DrawRectangle(0, 0, 600, 100, (Color){ 33, 150, 243, 255 });
            DrawTextEx(fontePrincipal, "EXTRATO BANCARIO", (Vector2){ 180, 35 }, 24, 2, WHITE);

            DrawRectangle(40, 80, 520, 100, WHITE);
            DrawRectangleLinesEx((Rectangle){ 40, 80, 520, 100 }, 1, LIGHTGRAY);
            DrawTextEx(fontePrincipal, "SALDO DISPONIVEL", (Vector2){ 230, 95 }, 14, 1, GRAY);
            
            const char* saldoTexto = TextFormat("R$ %s", FormatarDinheiro(saldo));
            Vector2 tamSaldo = MeasureTextEx(fontePrincipal, saldoTexto, 38, 2);
            DrawTextEx(fontePrincipal, saldoTexto, (Vector2){ 300 - tamSaldo.x/2, 120 }, 38, 2, (saldo >= 0 ? DARKGREEN : RED));

            DrawTextEx(fontePrincipal, "VALOR (use . para centavos):", (Vector2){ 215, 190 }, 12, 1, DARKGRAY);
            DrawRectangleRec(caixaTexto, WHITE);
            DrawRectangleLinesEx(caixaTexto, 2, caixaAtiva ? BLUE : LIGHTGRAY);
            DrawTextEx(fontePrincipal, textoEntrada, (Vector2){ caixaTexto.x + 10, caixaTexto.y + 8 }, 24, 1, BLACK);
            DrawRectangleRec(btnGanho, (Color){ 46, 125, 50, 255 });
            DrawTextEx(fontePrincipal, "ADICIONAR", (Vector2){ btnGanho.x + 50, btnGanho.y + 15 }, 18, 1, WHITE);
            DrawRectangleRec(btnGasto, (Color){ 198, 40, 40, 255 });
            DrawTextEx(fontePrincipal, "REMOVER", (Vector2){ btnGasto.x + 60, btnGasto.y + 15 }, 18, 1, WHITE);

            DrawTextEx(fontePrincipal, "ULTIMAS MOVIMENTACOES", (Vector2){ 50, 365 }, 16, 1, DARKGRAY);
            int inicio = (totalTransacoes > 7) ? totalTransacoes - 7 : 0;
            for (i = inicio; i < totalTransacoes; i++) {
                int posY = 400 + ((i - inicio) * 45);
                DrawRectangle(50, posY, 500, 40, WHITE);
                DrawRectangleLinesEx((Rectangle){ 50, posY, 500, 40 }, 1, (Color){ 230, 230, 230, 255 });
                
                Color cor = (historico[i].tipo == 'G') ? DARKGREEN : RED;
                DrawTextEx(fontePrincipal, (historico[i].tipo == 'G' ? "Entrada" : "Saida"), (Vector2){ 70, posY + 12 }, 14, 1, GRAY);
                
                const char* valFormatado = TextFormat("R$ %s", FormatarDinheiro(historico[i].valor));
                Vector2 tamVal = MeasureTextEx(fontePrincipal, valFormatado, 20, 1);
                DrawTextEx(fontePrincipal, valFormatado, (Vector2){ 530 - tamVal.x, posY + 10 }, 20, 1, cor);
            }

        EndDrawing();
    }

    f = fopen(ARQUIVO, "w");
    if (f) {
        fprintf(f, "%.2f %d\n", saldo, totalTransacoes);
        for (i = 0; i < totalTransacoes; i++) {
            fprintf(f, "%.2f %c\n", historico[i].valor, historico[i].tipo);
        }
        fclose(f);
    }
    
    UnloadFont(fontePrincipal);
    if (historico) free(historico);
    CloseWindow();
    
    return 0;
}
