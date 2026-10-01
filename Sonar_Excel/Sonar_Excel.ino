//***************************************************************************************************
//****************SONAR*****SONAR****SONAR*****SONAR*****SONAR*****SONAR*****SONAR*******************
//***************************************************************************************************
//*********************PROGRAMA DESENVOLVIDO PARA AVALIAR A VELOCIDADE DO CARRO**********************
//***************************************************************************************************
//*******************VERSÃO DO EXCEL PREPRARDA PARA RODAR TANTO EM 32 OU 64BITS**********************
//***OS EXEMPLOS DO PROGRAMA DO EXCEL DEVEM SER CONSTRUIDOS APARTIR DO EXEMPLO CONTIDO NESTA PASTA***
//*O PROGRMA ESCRITO PARA O EXCEL CONTEM UMA MACRO E O MESMO DEVE SER GRAVADO COM A TERMINAÇAO xlsm**
//*****************************ARQUIVOS DO EXCEL COM SUPORTE DE MACRO********************************
//******Triger Laranja (Pino 7)*****Echo Amarelo (Pino 5)*****Vermelho (+5V)******Marron (GND)*******
//***************************************************************************************10/05/2025**
//***************************************************************************************************

#include <VirtualWire.h>   //****BIBLIOTECA PARA ATIVAR O MODULO DO RADIO

//******DEFINIÇÕES DE HARDWARE
#define LED_01  13             //****SINALIZADOR GERAL    (LED VERMELHO) 
#define LED_02  12             //****SINALIZADOR DO RADIO (LED AZUL)
#define BUZ_01  8
#define trigPin 7              //****SONAR
#define echoPin 5              //****SONAR
#define pinRF  3               //****RADIO   

//*******DEFINIÇÃO DE VARIAVEIS
//char Letra [] = {'A','B','C','D'};  //Colunas do Excel
int i,opcao;
float Distancia,DDistancia[100],Tempo[100];
unsigned long int ttempo, tempo_inicial,tempo_final;
int NPontos = 50;      //*****NUMERO DE POSNTOS A SEREM COLETADOS
int metodo = 1;        //*****ESPECIFICA O TIPO DE MOVIMENTO DO CARRO (CASE QUE MOVIMENTARA O CARRO)
int Delta_T = 100;     //*****TEMPO ENTRE PONTOS COLETADOS PELO SONAR
int Delay_LED=2000;
float Vel_Som = 0.0385;
//*****************************************************
struct tipoPacote         //****PACOTE DE VARIAVEIS PARA O RADIO
{ 
    int  valor1;           //****NUMERO INTEIRO
};
tipoPacote pacote; 
//****************************************************
           void setup() 
        {
              Serial.begin(9600);
              pinMode(LED_01, OUTPUT);       //****LED DO PINO 13 LED VERMELHO
              pinMode(LED_02, OUTPUT);       //****LED DO PINO 12 LED AZUL
              pinMode(BUZ_01, OUTPUT);       //****BUZ DO PINO 11
              pinMode(trigPin, OUTPUT);
              pinMode(echoPin, INPUT);
              digitalWrite(BUZ_01, LOW);      //****BUZINA DESLIGADA
              
              vw_set_tx_pin(pinRF);       //****PINO ONDE ESTA CONECTADO O TRANSMISSOR
              vw_set_ptt_inverted(true);  //****ATIVANDO O MODULO TRANSMISSOR
              vw_setup(2000);             //****VELOCIDADE DE TRANSMISSÃO DE DADOS 
        }


void loop() 

  {
    
                  if (Serial.available()>0)  
                              
               {
                     opcao = Serial.read();    
               }
             
         switch(opcao)
         
     {

        case 'A':                       //*****MEDINDO A DISTANCIA DO SONAR AO CARRO*****
            Distancia = 0;          
            digitalWrite(LED_01, HIGH);

             for( i=0; i < 10; i= i+1)
         {
            digitalWrite(trigPin, LOW);                    //****Medindo a Distancia
            delayMicroseconds(10);   //2
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(20);   //10
            digitalWrite(trigPin, LOW);
            ttempo = pulseIn(echoPin, HIGH);
           Distancia = Distancia + (Vel_Som *(ttempo/2.0));   //Calculo da Distancia
            delay(200);
         }
            Distancia = Distancia/10.0;
            Serial.print("CELL,SET,");//Especifica a Celula do Excel
            Serial.print ("L");
            Serial.print (1);//Especifica a Linha
            Serial.print(",");
            Serial.println ("Distancia=");//Envia o Dado pata a Celula Especificada
            Serial.print("CELL,SET,");//Especifica a Celula do Excel
            Serial.print ("M");
            Serial.print (1);//Especifica a Linha
            Serial.print(",");
            Serial.println (Distancia,1);//Envia o Dado pata a Celula Especificada
            digitalWrite(LED_01, LOW);
            opcao=100;       
            break;

                 
case 'B':
             digitalWrite(LED_02, HIGH);                   //****LED AZUL
             digitalWrite(BUZ_01, HIGH);                   //****LED VERMELHO
             pacote.valor1 = metodo;                       //****TIPO DE MOVIMENTO****
             vw_send((uint8_t *)&pacote, sizeof(pacote));  //****ENVIA PELO TRANSMISSOR O METODO
             vw_wait_tx();                                 //****ESPERA A TRANSMISSÃO ACABAR 
             digitalWrite(LED_02, LOW);                    //****APAGA O LED AZUL*******
             digitalWrite(BUZ_01, LOW);
               
             Serial.println("RESETROW");
             Serial.println("LABEL,Tempo,Distancia,Tempo,Distancia");
                      
            digitalWrite(LED_01, HIGH);                     //****ACENEDE O LED VERMELHO
            delay(Delay_LED);                               //****PARA COMPENSAR O ATRASO GERADO PELOS LED DE SINALIZAÇÃO  NO CARRO
            tempo_inicial = millis();
            for( i=0; i < NPontos; i= i+1)
         {
            digitalWrite(trigPin, LOW);  //****Medindo a Distancia
            delayMicroseconds(2); 
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10); 
            digitalWrite(trigPin, LOW);
            ttempo = pulseIn(echoPin, HIGH);
            tempo_final = millis();
            Tempo[i] = (tempo_final-tempo_inicial)/1000.0;  //Tempo mo qual foi Realizada a Medida da Distancia
            DDistancia[i] =Vel_Som*(ttempo/2);             //Calculo da Distancia  em centimetros
            if (DDistancia[i]<20)
               {
                 digitalWrite(LED_02, HIGH);
                 digitalWrite(BUZ_01, HIGH);
                 pacote.valor1 = 255;                           //****COMANDO DE PARADA****
                 vw_send((uint8_t *)&pacote, sizeof(pacote));   //****ENVIA PELO TRANSMISSOR O VALOR 255
                 vw_wait_tx();  
                 digitalWrite(LED_02, LOW);                     //****ESPERA A TRANSMISSÃO ACABAR
                 digitalWrite(BUZ_01, LOW);
               }
            Serial.print("DATA,");
            Serial.print(Tempo[i]);                         //Tempo
            Serial.print(",");  
            Serial.println(Distancia-DDistancia[i], 1);     //Distancia              
            delay(Delta_T);                                 //****TEMPO ENTRE MEDIDAS
         }
            digitalWrite(LED_01, LOW);
                                                           
            digitalWrite(BUZ_01, HIGH);                    //************MANDA PARARA*************
            pacote.valor1 = 255;                           //****COMANDO DE PARADA****
            vw_send((uint8_t *)&pacote, sizeof(pacote));   //****ENVIA PELO TRANSMISSOR O VALOR 255
            vw_wait_tx();  
            delay(100);
            digitalWrite(BUZ_01, LOW);                     //****ESPERA A TRANSMISSÃO ACABAR
            opcao=100;    
            break;

              case 'D':    
                        //************ZERANDO A PLANILHA******************
                        digitalWrite(LED_01, HIGH);
                        Serial.println("RESETROW");
                        for( i=0; i < 100; i= i+1)
         {
                        Serial.print("DATA,");
                        Serial.print(0);
                        Serial.print(",");
                        Serial.println (0);
         }              
                        Serial.println("RESETROW");
                        digitalWrite(LED_01, LOW);
                       
                        opcao=100;
                        break;
                        
              
              case 'E':  
                        //************RECEBENDO PARAMETROS******************
                        digitalWrite(LED_01, HIGH); 
                        
                        metodo = Serial.parseInt();  //***Tipo de movimento
                        NPontos = Serial.parseInt(); 
                        Delta_T = Serial.parseInt();  
                        Delay_LED = Serial.parseInt();
                        
                        Serial.print("CELL,SET,");    //Especifica na Celula do Excel os valores do metodo como nde pontos, distância entre pontos...
                        Serial.print ("L");
                        Serial.print (2);//Especifica a Linha
                        Serial.print(",");
                        Serial.println ("Metodo=");//Envia o Dado pata a Celula Especificada
                        Serial.print("CELL,SET,");//Especifica a Celula do Excel
                        Serial.print ("M");
                        Serial.print (2);//Especifica a Linha
                        Serial.print(",");
                        Serial.println (metodo);//Envia o Dado pata a Celula Especificada
                        Serial.print("CELL,SET,");//Especifica a Celula do Excel
                        Serial.print ("L");
                        Serial.print (3);//Especifica a Linha
                        Serial.print(",");
                        Serial.println ("NPontos=");//Envia o Dado pata a Celula Especificada
                        Serial.print("CELL,SET,");//Especifica a Celula do Excel
                        Serial.print ("M");
                        Serial.print (3);//Especifica a Linha
                        Serial.print(",");
                        Serial.println (NPontos);//Envia o Dado pata a Celula Especificada
                        
                        Serial.print("CELL,SET,");//Especifica a Celula do Excel
                        Serial.print ("L");
                        Serial.print (4);//Especifica a Linha
                        Serial.print(",");
                        Serial.println ("Delta_T=");//Envia o Dado pata a Celula Especificada
                        Serial.print("CELL,SET,");//Especifica a Celula do Excel
                        Serial.print ("M");
                        Serial.print (4);//Especifica a Linha
                        Serial.print(",");
                        Serial.println (Delta_T);//Envia o Dado pata a Celula Especificada
                        
                        digitalWrite(LED_01, LOW); 
                        
                        for( i=0; i < 10; i= i+1)
                    {
                        digitalWrite(LED_01, HIGH);   // LED Vermelho
                        delay(50);
                        digitalWrite(LED_01, LOW);
                        delay(50);
                    }   
                        opcao=100;
                        break;
                      
                   default:
                   delay(1);                
     }

}       