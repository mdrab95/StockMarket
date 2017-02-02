#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>



typedef struct Struktura		// ta struktura przechowuje wskaznik ktory bedzie rezerwowal pamiec dla tej struktury dynamicznej, jest to 'float *Element',
								// a takze zmienna pamietajaca 'rozmiar' tej struktury - 'int rozmiar'.
{
	float *Element;
	int rozmiar;
} Struktura;




void PrepareStruktura(Struktura* Element)
{
	Element->rozmiar = 0;
	Element->Element = NULL;
}



void Push(Struktura *Element, float f)		// dodaje element do struktury, przydziela dodatkowa pamiec oraz zwieksza wartosc liczby 'rozmiar'
{
     Element->rozmiar += 1;
    Element->Element = (float*)realloc(Element->Element,Element->rozmiar * sizeof(f));
	Element->Element[Element->rozmiar-1] = f;
}


void ZwolnijStruktura(Struktura *Element)		// funkcja zwalniajaca pamiec
{
	Element->rozmiar = 0;
	free(Element->Element);
}



void Argumenty(char** argv,int argc, int* input_file, int* step, int* output_srednia, int* odchylenie)  // funkcja nadajaca wartosci parametrom
{
  	 int count = 0;
 	
for(count = 0; count<argc; ++count)
{
    if(strcmp( argv[count], "-i" )==0)
      	*input_file = count+1;
   	else if(strcmp( argv[count], "-k" )==0)
   		*step = count+1;
   	else if(strcmp ( argv[count], "-s")==0)
   		*output_srednia = count+1;
   	else if(strcmp( argv[count], "-o" )==0)
   		*odchylenie = count+1;
}


}



void WczytajZPliku(char* input_filename,Struktura* v)  // wczytywanie z pliku do struktury
{
    FILE *input_file = fopen(input_filename, "r");
    
    float liczba;
    while(!feof(input_file))
    {
    	fscanf(input_file,"%f",&liczba);
    	Push(v,liczba);
    }

    fclose(input_file);
}



int StrNaInt(char* str)		// zamienia str na int
{
	int len = strlen(str);
	int i;
	int result = 0;
	for(i=0;i<len;++i)
	{
		result = result * 10;
		result = result + str[i] - '0';
	}
	return result;
}



float SredniaRuch(int step, int t,Struktura* v)	// funkcja na ruchoma srednia (moving average)
{
	int i;
	float sum = 0;
	for(i=t-step+1;i<=t;++i)
		sum = sum + v->Element[i];
	return sum/step;
}



float Odchylenie(float avg, int step, int t, Struktura* v)	// funkcja na odchylenie
{
	int i;
	float sum = 0;
	for(i=t-step+1;i<=t;++i)
		sum = sum + (avg - v->Element[i])*(avg - v->Element[i]);
	return sqrt(sum/step);
}



void StworzPlik(char* out)	// funkcja ktora tworzy plik
{
	FILE* outfile = fopen(out,"w");
	fclose(outfile);
}



void BledneMozliwosci(char* out)  // wypisuje informacje w przypadku gdy ilosc danych nie wystarczy do obliczenia sredniej, odchylenia
{
	FILE *outfile = fopen(out,"a");
	fprintf(outfile,"Blad - nie wystarczajaco danych do obliczenia.\n");
	fclose(outfile);
}



void ZapiszDane(char* str,float data)   // zapis do pliku
{
	FILE *out = fopen(str,"a");
	fprintf(out,"%f\n",data);
	fclose(out);
}

//     3 przypadki - pierwszy sprawdza czy nie brakuje parametru
//					 drugi sprawdza czy nie brakuje pliku
//     				 trzeci - gdy brakuje nazwy pliku lub jest podana jako nastepny parametr


int ZleDane(int input_file, int step, int output_srednia, int odchylenie, int argc)
{
    // 1 przypadek
    if( input_file == 0 || step == 0 || output_srednia == 0 || odchylenie == 0 )
	 return 1;
	
    // 2 przypadek
	if( !(step < argc) ||  !(input_file < argc) || !(output_srednia < argc) || !(odchylenie < argc))
	  return 1;
    
    // 3 przypadek
   	if(step+1==input_file || step+1 == output_srednia || step+1 == odchylenie)
     return 1;
     
     if(input_file+1==step || input_file+1 == output_srednia || input_file+1 == odchylenie)
     return 1;
     if(output_srednia+1==input_file || output_srednia+1 == step || output_srednia+1 == odchylenie)
     return 1;
     if(odchylenie+1==input_file || odchylenie+1 == output_srednia || odchylenie+1 == step)
     return 1;	
    
 return 0;
}



void ZapiszBledy(char* out1, char* out2, int how_many)		// zapisuje informacje o bledach (za malo danych do obliczenia)
{
     int t;
	for(t=0; t<how_many; ++t)
	{
		BledneMozliwosci(out1);
		BledneMozliwosci(out2);
	}
}


// i oto nadchodzi wspaniala funkcja main

int main(int argc, char**argv)
{
	
	 int input_file = 0;
	 int step = 0;
	 int output_srednia = 0;
	 int odchylenie = 0;
	 int t;
	 Struktura Element;
	 PrepareStruktura(&Element); 
	 
	 Argumenty(argv,argc,&input_file,&step,&output_srednia,&odchylenie);
	 	
   if(ZleDane(input_file,step,output_srednia,odchylenie,argc))
    {   
        printf("Za malo argumentow" );
	       return 1;
    };
	
	
	WczytajZPliku(argv[input_file],&Element); // wczytywanie z pliku 
		
	StworzPlik(argv[output_srednia]);		// tworzenie plikow output
	StworzPlik(argv[odchylenie]);

	
	ZapiszBledy(argv[output_srednia],argv[odchylenie],StrNaInt(argv[step])-1); // niechciane przypadki - zbyt malo danych
	

	
	for(t=StrNaInt(argv[step])-1;t<Element.rozmiar;++t)		// obliczenia i zapis do plikow wyjsciowych
	{
		ZapiszDane(argv[output_srednia],SredniaRuch(StrNaInt(argv[step]),t,&Element));
		ZapiszDane(argv[odchylenie],Odchylenie(SredniaRuch(StrNaInt(argv[step]),t,&Element),StrNaInt(argv[step]),t,&Element));
	}
	
	printf("Wszystko poszlo tak jak trzeba, otworz pliki by zobaczyc wyniki!");

	ZwolnijStruktura(&Element);
	 return 0;
	

}



