/*
 已知bug:
 Player[1].B2B不明原因更動  fixed
 被KO時不明原因部分方塊消失  fixed
 被KO時，無法啟動forceend   fixed
 Hold的時候不會判斷重疊 
 combo 聲音不連貫 
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <thread>
using namespace std;
typedef long long ll;
    sf::Music MainM,DropM,ComboM[2],ClrM,HoldM,KOM,KOgetM,TimeM;//MainM includes StartM, TimeM, win lose
    
int digitof(int a){
	if(a<10) return 1;
	else return(digitof(a/10)+1);
}
enum BlockType {
    Z=1, L=2, O=3, S=4, I=5, J=6, T=7,
    D=8, W=9, B1=10, B2=11, B3=12
};
string BlockTypeR(int a) {
    switch(a) {
        case 1:    return "Z";        
        case 2:    return "L";
        case 3:    return "O";
        case 4:    return "S";
        case 5:    return "I";
        case 6:    return "J";
        case 7:    return "T";
        case 8:    return "D";
        case 9:    return "W";
        case 10:   return "B1";
        case 11:   return "B2";
        case 12:   return "B3";
        default:   return 0;
    }
};

sf::Texture blkT[12],numT[10];
int playernum=1,forceend=0,record=0;
FILE* recin,*recout; sf::Clock MainC;

struct xy {
    int x;
    int y;
    void s(int px,int py) {
        x=px,y=py;
    }
};

class BlockInfo {
    public:
        int Blocktype=0,status=0;
        string Blockname;
        int size=0;
        xy pos[4];//relative
        xy posc,poscD;
        sf::Sprite blkS,blkDS;
        sf::Texture T,DT;
        
        //turn 0:clockwise 1:counterclockwise
		//sf::Texture blkT[2];
		
		
        void createD(){
			//blkS.
        	sf::RenderTexture RT;
        	switch(size){
        		case 1: RT.create(23,23); break;
        		case 2: RT.create(46,46); break;
        		case 3: RT.create(69,69); break;
        		case 4: RT.create(92,92); break;     		
			}
			RT.clear(sf::Color(0,0,0,0));
			blkDS.setTexture(blkT[BlockType::D],true);
			for(int i=0;i<4;i++){
				blkDS.setPosition(23*(1+pos[i].x),23*(1-pos[i].y));
				RT.draw(blkDS);
			}
			RT.display();
			DT=RT.getTexture();
			blkDS.setTexture(DT,true);
			
			//RT.
		}
		
        void create(void){//////////////////////////
        	//blkS.
        	sf::RenderTexture RT;        	
        	switch(size){
        		case 1: RT.create(23,23); break;
        		case 2: RT.create(46,46); break;
        		case 3: RT.create(69,69); break;
        		case 4: RT.create(92,92); break;     		
			}
			RT.clear(sf::Color(0,0,0,0));
			blkS.setScale(1,1); blkS.setOrigin(0,0);
			blkS.setTexture(blkT[this->Blocktype],true);
			for(int i=0;i<4;i++){
				blkS.setPosition(23*(1+pos[i].x),23*(1-pos[i].y));
				RT.draw(blkS);
			}
			RT.display();
			T=RT.getTexture();
			blkS.setTexture(T,true);
			blkS.setPosition(200,360);///////////////
			
			createD();
			//RT.		
		}
		
        /*BlockInfo(int a) {
        	reset(a);
        }*/
        void reset(int a,int sta=-1){
        	this->Blocktype=a;
            if(sta==-1){
            	this->posc.x=5;
          	    this->posc.y=19;
			}
            if(sta<0||sta>=4) sta=0; 
            this->status=sta;
            switch(a) {
                case 1:
                    Blockname="Z";size=3;pos[0].s(-1,1);pos[1].s(0,1);pos[2].s(0,0);pos[3].s(1,0);break;
                case 2:
                    Blockname="L";size=3;pos[0].s(-1,0);pos[1].s(0,0);pos[2].s(1,0);pos[3].s(1,1);break;
                case 3:
                    Blockname="O";size=2;pos[0].s(0,0);pos[1].s(-1,0);pos[2].s(-1,1);pos[3].s(0,1);break;
                case 4:
                    Blockname="S";size=3;pos[0].s(-1,0);pos[1].s(0,0);pos[2].s(0,1);pos[3].s(1,1);break;
                case 5:
                    Blockname="I";size=4;pos[0].s(-1,0);pos[1].s(0,0);pos[2].s(1,0);pos[3].s(2,0);break;
                case 6:
                    Blockname="J";size=3;pos[0].s(-1,1);pos[1].s(-1,0);pos[2].s(0,0);pos[3].s(1,0);break;
                case 7:
                    Blockname="T";size=3;pos[0].s(0,0);pos[1].s(-1,0);pos[2].s(0,1);pos[3].s(1,0);break;
                case 8: Blockname="D";break;
                case 9: Blockname="W";break;
                case 10: Blockname="B1";break;
                case 11: Blockname="B2";break;
                case 12: Blockname="B3";break;                
                default:
                	cout<<"Error in BlockInfo/reset";
                    break;
            }
            switch(a) {               
                 
                case 8: 
                case 9: 
                case 10: 
                case 11: 
				case 12:               
                    size=1;pos[0].s(0,0);pos[1].s(0,0);pos[2].s(0,0);pos[3].s(0,0);break;
                default:
                    break;
            }
            while(sta-->0){			
         	   if(size==3) {
                  for(int i=0; i<4; i++) {
                     swap(pos[i].x,pos[i].y);
                     pos[i].y=-pos[i].y;
                  }                
               } else if(size==4) {
                  for(int i=0;i<4; i++) {
                     swap(pos[i].x,pos[i].y);
       	             pos[i].y=-pos[i].y;
                     pos[i].x++;
      	          }                
       	       }else break;			
   		    }
            this->create();
            //blkT[0].loadFromFile("Node/"+Blockname+".png");
            //blkT[1].loadFromFile("Node/D.png");
			}
};

xy KW3R[4][5];
xy KW4R[4][5];

//int B2Bplayer2=0;

class PlayerInfo{
    public:
    	PlayerInfo* Opponent=NULL;
        int KO=0,forcelose=0,PC=0,KOt=0;//PC:Perfectclaer  KOt:be KOed x times by the opponent
		int KOed=0;//KOed status
        int Linesent=0;
        char Field[400];//////////????y first
        int Combo=-1;
        int Lineclr=0;//how many lines cleared in one drop 
        int Bomb=0;//bomb already appear
        int BombRB=0;//red bar
        int BombOB=0;//orange bar
        int Highest=0;
        int Playername=0;//0:myself
        //char NextB[7];//0:now
        BlockInfo NextB[7];//0:now
        //BlockInfo* Block;//now
        BlockInfo HoldB;//HoldB.Blocktype=-1;
		sf::Sprite FieldS,ComboS,LineS,TimeS,KOS,FdallS,BarS,PCS;//Fdall contains: Field, Combo, Time, NextB[0].blkS, NextB[0].blkDS, Bomb    Time contains: KOed, win/lose , time's up, 
		sf::Texture FieldT,ComboT,LineT,TimeT,KOT,FdallT,BarT,PCT;
		sf::RectangleShape DarkS;
        int bagnow=0;//take the nth item in bag to NextB
        char bag[7];
        //int Hold=-1;
        int Holdyet=0;// 1:yes  bool
        int B2B=0,SpecialTurn=0,Tspin;
        sf::Clock dwnclk;//auto
        sf::Clock dwnPclk;//press time
        sf::Clock movclk,Comboclk,KOclk,Barclk,PCclk;
        int movdir=-1,dwnP=0;//0:right 1:left -1:none
        int rectim=0;//the time in record
        
        /*PlayerInfo(){//create bag and sprite
     	  	 this->reset();
	    }*/
	int Fd(int ty,int tx){
		return ty*10+tx;
	}
   void KOreset(void){//when being KO, reset
  	 	KOed=1; KOt++;
	   Highest-=Bomb;
        memmove(Field,Field+10*Bomb,sizeof(char)*10*(Highest+1));
        memset(Field+(Highest+1)*10,0,sizeof(char)*10*(39-Highest));
        Bomb=0;
        if(Playername==0){
        	KOM.stop(); KOM.play();
		} 
        TimeT.loadFromFile("KO/KO.png");
        TimeS.setTexture(TimeT);
        TimeS.setPosition(160+484*Playername,364);        
        KOclk.restart();
        Opponent->KOget();
        if(Opponent->KO>=10) forceend=Playername+1;
        /*RField();
        Refresh();       */
    }
    void KOget(){
    	KO++; 
		if(Playername==0){
    	KOgetM.stop(); KOgetM.play();	
		}
    	KOT.loadFromFile("KO/"+to_string(KO)+".png");
    	KOS.setTexture(KOT,true);
    	KOS.setPosition(32+484*Playername,300);
	}
	void clrL(int Line){//enter which line to delete
			memmove(Field+10*Line,Field+10*(Line+1),sizeof(char)*10*(39-Line));
			memset(Field+390,0,sizeof(char)*10);
			Highest--;
	}
	
    void Next(void){
    	//delete(NextB[0]);
    	for(int i=0;i<6;i++){///////////////////////////////////////
    		NextB[i]=NextB[i+1];
    		NextB[i].reset(NextB[i].Blocktype);
		}		
        //memmove(NextB,NextB+1,sizeof(char)*6);
        if(record&&Playername==0) fprintf(recout," %d\n",bag[bagnow]);
        else if(record&&Playername==1) fscanf(recin,"%d",&bag[bagnow]);
        NextB[6].reset(bag[bagnow]);
        
        bagnow++;        
        if(bagnow==7){
            bagnow=0;
            for(int i=0;i<20;i++){
                swap(bag[rand()%7],bag[rand()%7]);
            }
        }
        /*NextB[1].blkS.setPosition(406-11.5*NextB[1].size,240-11.5*NextB[1].size);
        NextB[2].blkS.setPosition(406-11.5*NextB[2].size,325-11.5*NextB[2].size);
        NextB[3].blkS.setPosition(406-11.5*NextB[3].size,394-11.5*NextB[3].size);
        NextB[4].blkS.setPosition(406-11.5*NextB[4].size,461-11.5*NextB[4].size);
        NextB[5].blkS.setPosition(406-11.5*NextB[5].size,524-11.5*NextB[5].size);*/
        JudgeDead();
        setPos();
		//NextS[4].setTexture()///////////////
    }
    void setPos(){
    	for(int i=1;i<=5;i++){
    		if(NextB[i].size==4) NextB[i].blkS.setOrigin(46,34.5);
    		if(NextB[i].size==3) NextB[i].blkS.setOrigin(34.5,23);
    		if(NextB[i].size==2) NextB[i].blkS.setOrigin(23,23);
		}
		switch(HoldB.size){
			case 4:
				HoldB.blkS.setOrigin(46,34.5); break;
			case 3:
				HoldB.blkS.setOrigin(34.5,23); break;
			case 2:
				HoldB.blkS.setOrigin(23,23); break;		
		}
		
		//NextB[0].blkS.setScale(1,1); NextB[0].blkS.;
    	NextB[1].blkS.setScale(0.8,0.8); NextB[1].blkS.setPosition(406+484*Playername,240);
        NextB[2].blkS.setScale(0.75,0.75); NextB[2].blkS.setPosition(406+484*Playername,325);
        NextB[3].blkS.setScale(0.6,0.6); NextB[3].blkS.setPosition(406+484*Playername,394);
        NextB[4].blkS.setScale(0.5,0.5); NextB[4].blkS.setPosition(406+484*Playername,461);
        NextB[5].blkS.setScale(0.5,0.5); NextB[5].blkS.setPosition(406+484*Playername,524);
        HoldB.blkS.setScale(0.8,0.8);    HoldB.blkS.setPosition(62+484*Playername,240);
	}
    void Hold(){/////////
    	if(Playername>=playernum) return;
    	if(this->Holdyet==1) return;
	    this->Holdyet=1;
	    dwnclk.restart();
    	if(this->HoldB.Blocktype==0){
    		HoldB.reset(NextB[0].Blocktype);
    		if(record&&Playername==0) fprintf(recout,"%d h",MainC.getElapsedTime().asMilliseconds()-KOt*1000);
    		Next();
			return;
		}
		if(record&&Playername==0) fprintf(recout,"%d h\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000);
		swap(HoldB,NextB[0]);
		HoldB.reset(HoldB.Blocktype); NextB[0].reset(NextB[0].Blocktype);
		adjusty();
		switch(HoldB.size){
			case 4:
				HoldB.blkS.setOrigin(46,34.5); break;
			case 3:
				HoldB.blkS.setOrigin(34.5,23); break;
			case 2:
				HoldB.blkS.setOrigin(23,23); break;		
		}HoldB.blkS.setScale(0.8,0.8);    HoldB.blkS.setPosition(62+484*Playername,240);
    	return;
	}
	void getBomb(int a){//get bombs from the opponent
		BombRB=BombOB;
		BombOB+=a;
		Barclk.restart();
	}
	void addBomb(int a){//add bombs to the field
		if(Bomb+a>20) a=20-Bomb;
		memmove(Field+a*10,Field,sizeof(char)*10*(Highest+1));
		for(int i=0;i<a*10;i++){
			Field[i]=BlockType::W;			
		}
		for(int i=0;i<a;i++){
			Field[Fd(i,rand()%10)]=BlockType::B2;			
		}
		Bomb+=a; Highest+=a;
	}
	void reset(int a,int temp,PlayerInfo* opp){// temp: initial bombs		
		Opponent=opp;
        forcelose=0;
		Playername=a;
		KO=0; KOed=0; KOt=0;
        Linesent=0;
        memset(Field,0,sizeof(char)*40*10);//////////????y first[21][10]
        Combo=-1;
        Lineclr=0;//how many lines cleared in one drop 
        Bomb=0;//bomb already appear
        BombRB=0;//red bar
        BombOB=0;//orange bar
        Highest=-1;
        B2B=0;PC=0;
        //BlockInfo NextB[7];//0:now
        rectim=0;//the time in record
        HoldB.Blocktype=0;
		//sf::Sprite Board;//the sprite of whole board
        bagnow=0;//take the nth item in bag to NextB
        //char bag[7];
        //int Hold=-1;
        Holdyet=0;// 1:yes
        
        //sf::Clock dwnclk;
        //sf::Clock movclk;
        movdir=-1;//0:right 1:left
        for(int i=0;i<7;i++){
        	bag[i]=i+1;
        }
        for(int i=0;i<20;i++){
            swap(bag[rand()%7],bag[rand()%7]);
            //swap(bag2[rand()%7],bag2[rand()%7]);
        }
        for(int i=0;i<7;i++){
        	if(record){
        		if(Playername==0){
        			if(i==0) fprintf(recout,"%d",bag[i]);
        			else if(i==6) fprintf(recout," %d\n",bag[i]);
					else fprintf(recout," %d",bag[i]);
				}else if(playernum==2&&Playername==1){
					if(!fscanf(recin,"%d",&bag[i])) cout<<"ERRor";
					if(i==6) fscanf(recin,"%d",&this->rectim);
					printf(" %d",bag[i]);//// 
					if(i==6) printf(" %d",rectim);//// 
				}       		
			}
        	NextB[i].reset(bag[i]);
		}
        
        for(int i=0;i<20;i++){
            swap(bag[rand()%7],bag[rand()%7]);
        }
        //memset(Field,-1,sizeof(char)*10*21);
        /*NextB[1].blkS.setPosition(406-11.5*NextB[1].size,240-11.5*NextB[1].size);
        NextB[2].blkS.setPosition(406-11.5*NextB[2].size,325-11.5*NextB[2].size);
        NextB[3].blkS.setPosition(406-11.5*NextB[3].size,394-11.5*NextB[3].size);
        NextB[4].blkS.setPosition(406-11.5*NextB[4].size,461-11.5*NextB[4].size);
        NextB[5].blkS.setPosition(406-11.5*NextB[5].size,524-11.5*NextB[5].size);*/
        RLine();
        //test
        if(temp>15) temp=15;
        if(temp<0) temp=0;
        for(int i=0;i<temp;i++){
        	for(int j=0;j<10;j++){
        		Field[Fd(i,j)]=BlockType::W;
			}
			Field[Fd(i,rand()%10)]=BlockType::B2;
		}
		Bomb=temp;Highest=temp;
		DarkS.setSize(sf::Vector2f(230,463));
		DarkS.setFillColor(sf::Color(0,0,0,128));
		DarkS.setPosition(0,0);
        RField();
        setPos();
	}
	/*
		J, L, S, T, Z Tetromino Wall Kick Data
Test 1	Test 2	Test 3	Test 4	Test 5
0>>1	( 0, 0)	(-1, 0)	(-1, 1)	( 0,-2)	(-1,-2)
1>>0	( 0, 0)	( 1, 0)	( 1,-1)	( 0, 2)	( 1, 2)
1>>2	( 0, 0)	( 1, 0)	( 1,-1)	( 0, 2)	( 1, 2)
2>>1	( 0, 0)	(-1, 0)	(-1, 1)	( 0,-2)	(-1,-2)
2>>3	( 0, 0)	( 1, 0)	( 1, 1)	( 0,-2)	( 1,-2)
3>>2	( 0, 0)	(-1, 0)	(-1,-1)	( 0, 2)	(-1, 2)
3>>0	( 0, 0)	(-1, 0)	(-1,-1)	( 0, 2)	(-1, 2)
0>>3	( 0, 0)	( 1, 0)	( 1, 1)	( 0,-2)	( 1,-2)

I Tetromino Wall Kick Data
Test 1	Test 2	Test 3	Test 4	Test 5
0>>1	( 0, 0)	(-2, 0)	( 1, 0)	(-2,-1)	( 1, 2)
1>>0	( 0, 0)	( 2, 0)	(-1, 0)	( 2, 1)	(-1,-2)
1>>2	( 0, 0)	(-1, 0)	( 2, 0)	(-1, 2)	( 2,-1)
2>>1	( 0, 0)	( 1, 0)	(-2, 0)	( 1,-2)	(-2, 1)
2>>3	( 0, 0)	( 2, 0)	(-1, 0)	( 2, 1)	(-1,-2)
3>>2	( 0, 0)	(-2, 0)	( 1, 0)	(-2,-1)	( 1, 2)
3>>0	( 0, 0)	( 1, 0)	(-2, 0)	( 1,-2)	(-2, 1)
0>>3	( 0, 0)	(-1, 0)	( 2, 0)	(-1, 2)	( 2,-1)
*/
		
		void turnR() {
			if(Playername>=playernum||KOed) return;
       		int success=0;
         	if(NextB[0].size==3) {
                for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].y=-NextB[0].pos[i].y;
                }
                for(int j=0; j<5; j++) {
                	int tX=NextB[0].posc.x+KW3R[NextB[0].status][j].x,tY=NextB[0].posc.y+KW3R[NextB[0].status][j].y;
                    for(int i=0;i<4;i++){
                    	int ttX=tX+NextB[0].pos[i].x,ttY=tY+NextB[0].pos[i].y;
                    	if(ttX<0||ttX>=10||ttY<0||Field[Fd(ttY,ttX)]!=0) break;
                    	if(i==3){
                    		if(j==0) SpecialTurn=0; else SpecialTurn=1;
							success=1;                   		
						}
					}
					if(success) {
						NextB[0].posc.x+=KW3R[NextB[0].status][j].x; NextB[0].posc.y+=KW3R[NextB[0].status][j].y; break;					
					}
                }
                if(!success){
                	for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].x=-NextB[0].pos[i].x;
              	  }
				}
            } else if(NextB[0].size==4) {
                for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].y=-NextB[0].pos[i].y;
                    NextB[0].pos[i].x++;
                }
                for(int j=0; j<5; j++) {
                	int tX=NextB[0].posc.x+KW4R[NextB[0].status][j].x,tY=NextB[0].posc.y+KW4R[NextB[0].status][j].y;
                    for(int i=0;i<4;i++){
                    	int ttX=tX+NextB[0].pos[i].x,ttY=tY+NextB[0].pos[i].y;
                    	if(ttX<0||ttX>=10||ttY<0||Field[Fd(ttY,ttX)]!=0) break;
                    	if(i==3){
                    		if(j==0) SpecialTurn=0; else SpecialTurn=1;
                    		success=1;                   		
						}
					}
					if(success) {
						NextB[0].posc.x+=KW4R[NextB[0].status][j].x; NextB[0].posc.y+=KW4R[NextB[0].status][j].y; break;
					}
                }
                if(!success){
                	for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].x=-NextB[0].pos[i].x;
                    NextB[0].pos[i].y--;
           		    }
				}
            }else return;
            
            if(success){
            	NextB[0].create();
            	if(++NextB[0].status==4) NextB[0].status=0;/////////////not sure
            	if(Playername==0&&record){
					if(SpecialTurn==0) fprintf(recout,"%d r %d\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].status);
					else fprintf(recout,"%d r %d m %d %d\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].status,NextB[0].posc.x,NextB[0].posc.y-Bomb);
				}
            	Tspin=1;
			}
			
        }
        void turnL() {
        	if(Playername>=playernum||KOed) return;
        	int success=0;
        	if(--NextB[0].status<0) NextB[0].status=3;/////////////not sure
            if(NextB[0].size==3) {
                for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].x=-NextB[0].pos[i].x;
                }
                for(int j=0; j<5; j++) {
                	int tX=NextB[0].posc.x-KW3R[NextB[0].status][j].x,tY=NextB[0].posc.y-KW3R[NextB[0].status][j].y;///it's minus
                    for(int i=0;i<4;i++){
                    	int ttX=tX+NextB[0].pos[i].x,ttY=tY+NextB[0].pos[i].y;
                    	if(ttX<0||ttX>=10||ttY<0||Field[Fd(ttY,ttX)]!=0) break;
                    	if(i==3){
                    		if(j==0) SpecialTurn=0; else SpecialTurn=1;
                    		success=1;                   		
						}
					}
					if(success) {
						NextB[0].posc.x-=KW3R[NextB[0].status][j].x; NextB[0].posc.y-=KW3R[NextB[0].status][j].y; break;
					}
                }
                if(!success){
                	for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].y=-NextB[0].pos[i].y;
           	        } 
		    	}
            } else if(NextB[0].size==4) {
                for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].x=-NextB[0].pos[i].x;
                    NextB[0].pos[i].y--;
                }
                for(int j=0; j<5; j++) {
                	int tX=NextB[0].posc.x-KW4R[NextB[0].status][j].x,tY=NextB[0].posc.y-KW4R[NextB[0].status][j].y;///it's minus
                    for(int i=0;i<4;i++){
                    	int ttX=tX+NextB[0].pos[i].x,ttY=tY+NextB[0].pos[i].y;
                    	if(ttX<0||ttX>=10||ttY<0||Field[Fd(ttY,ttX)]!=0) break;
                    	if(i==3){
                    		if(j==0) SpecialTurn=0; else SpecialTurn=1;
                    		success=1;                   		
						}
					}
					if(success) {
						NextB[0].posc.x-=KW4R[NextB[0].status][j].x; NextB[0].posc.y-=KW4R[NextB[0].status][j].y; break;
					}
                }
                if(!success){
                	for(int i=0; i<4; i++) {
                    swap(NextB[0].pos[i].x,NextB[0].pos[i].y);
                    NextB[0].pos[i].y=-NextB[0].pos[i].y;
                    NextB[0].pos[i].x++;
           		    }
				}
            } else return;//size==2
            if(success){
            	NextB[0].create();Tspin=1;
            	if(Playername==0&&record){
					if(SpecialTurn==0) fprintf(recout,"%d r %d\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].status);
					else fprintf(recout,"%d r %d m %d %d\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].status,NextB[0].posc.x,NextB[0].posc.y-Bomb);
				}
			}
        }
        void Refresh(){
        	int out=0;
        	NextB[0].poscD.x=NextB[0].posc.x;NextB[0].poscD.y=NextB[0].posc.y;
        	while(out==0){
        		NextB[0].poscD.y--;
        		for(int i=0;i<4;i++){
        			int tX=NextB[0].poscD.x+NextB[0].pos[i].x,tY=NextB[0].poscD.y+NextB[0].pos[i].y;
        			if(tY<0||Field[Fd(tY,tX)]!=0) {
        				out=1;break;
					}
				}        		
        		
			}
			NextB[0].poscD.y++;
			//NextB[0].blkS.setPosition(114+23*(NextB[0].posc.x-1)+Playername*484,180+23*(18-NextB[0].posc.y));
			//NextB[0].blkDS.setPosition(114+23*(NextB[0].poscD.x-1)+Playername*484,180+23*(18-NextB[0].poscD.y));
			
			sf::RenderTexture RT;
			RT.create(230,463);
			RT.clear(sf::Color(0,0,0,0));
			//FdallS.setColor();
			FdallS.setTexture(FieldT,true);
			FdallS.setPosition(0,0);
			RT.draw(FdallS);
			NextB[0].blkDS.setPosition(23*(NextB[0].poscD.x-1),3+23*(18-NextB[0].poscD.y));
			NextB[0].blkS.setPosition(23*(NextB[0].posc.x-1),3+23*(18-NextB[0].posc.y));
			
			
			
			
			if(PC){
        	RPC(); RT.draw(PCS);
			}
			if(Combo>0&&Comboclk.getElapsedTime().asMilliseconds()<1000){
        	RCombo(); RT.draw(ComboS);
			}
			
			if(!KOed) RT.draw(NextB[0].blkDS);//DS first
			RT.draw(NextB[0].blkS);
			if(KOed) {
				RT.draw(DarkS); RT.draw(TimeS);
			}
			
				
			RT.display();			
			FdallT=RT.getTexture();			
			FdallS.setTexture(FdallT,true);
			FdallS.setPosition(114+484*Playername,177);			
		}
		void movR(){
			if(Playername>=playernum||KOed) return;
			int suc=1;
			NextB[0].posc.x++;
        		for(int i=0;i<4;i++){
        			int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        			if(tX>=10||Field[Fd(tY,tX)]!=0) {
        				NextB[0].posc.x--;suc=0;break;
					}
				}
			if(suc) {
				Tspin=0; if(record)recordM();
			}
		}
		void movL(){
			if(Playername>=playernum||KOed) return;
			int suc=1;
			NextB[0].posc.x--;
        		for(int i=0;i<4;i++){
        			int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        			if(tX<0||Field[Fd(tY,tX)]!=0) {
        				NextB[0].posc.x++;suc=0;break;
					}
				}      		
			if(suc) {
				Tspin=0; if(record)recordM();
			}
		}
		void movD(){
			if(Playername>=playernum||KOed) return;
			int suc=1;			
			NextB[0].posc.y--;
        		for(int i=0;i<4;i++){
        			int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        			if(tY<0||Field[Fd(tY,tX)]!=0) {
        				NextB[0].posc.y++; suc=0;
        				if(this->dwnclk.getElapsedTime()>sf::milliseconds(1000)){
        					Drop();
						}		
						break;
					}
				}
			if(suc) {
				Tspin=0; this->dwnclk.restart(); if(record)recordM();
			}			
		}
		void recordM(){
			if(record&&Playername==0){
				fprintf(recout,"%d m %d %d\n",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].posc.x,NextB[0].posc.y-Bomb);
				
			}
		}
		
        void Drop(){
        	if(Playername>=playernum) return;
        	if(this->KOed) return;
        	this->Holdyet=0;
            this->dwnclk.restart();
            this->movclk.restart();
        	this->Lineclr=0;
        	int out=0,originaly=NextB[0].posc.y;       
			
			 	
			 	
			if(Playername==0||!record){
       	 		while(out==0){
        			NextB[0].posc.y--;
        			for(int i=0;i<4;i++){
        				int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        				if(tY<0||Field[Fd(tY,tX)]!=0) {
        					out=1;break;
		   		     	}
					}
				}
				NextB[0].posc.y++;
				if(NextB[0].posc.y!=originaly) Tspin=0;
				if(Tspin&&NextB[0].Blocktype==BlockType::T){//handle T-spin
					Tspin=0;
					int tX=NextB[0].posc.x,tY=NextB[0].posc.y;
					int losttype=0;
					if(tX==9||Field[Fd(tY+1,tX+1)]!=0) Tspin++;else losttype=1;
					if(tX==0||Field[Fd(tY+1,tX-1)]!=0) Tspin++;else  losttype=2;
					if(tX==0||tY==0||Field[Fd(tY-1,tX-1)]!=0) Tspin++;else  losttype=3;
					if(tX==9||tY==0||Field[Fd(tY-1,tX+1)]!=0) Tspin++;else  losttype=4;
					if(Tspin>=3) {
						if(Tspin==4){
							Tspin=2;
						}else Tspin=1;
						switch(NextB[0].status){
							case 0:
								if(losttype==3||losttype==4) Tspin=2; break;
							case 1:
								if(losttype==2||losttype==3) Tspin=2; break;
							case 2:
								if(losttype==1||losttype==2) Tspin=2; break;
							case 3:
								if(losttype==1||losttype==4) Tspin=2; break;
						}
					}
					//Tspin: Tspin=2  Tspin mini=1
					else Tspin=0;
				} else Tspin=0;
				fprintf(recout,"%d d %d %d %d",MainC.getElapsedTime().asMilliseconds()-KOt*1000,NextB[0].posc.x,NextB[0].posc.y-Bomb,Tspin);
			}else if(record&&Playername==1){
				fscanf(recin,"%d%d%d",&NextB[0].posc.x,&NextB[0].posc.y,&Tspin);
				printf(" %d %d %d",NextB[0].posc.x,NextB[0].posc.y,Tspin);
				NextB[0].posc.y+=Bomb;
			}
			 
			 
			 
			
			int Bombx=-1,Bombclr=0,point=0;
        	for(int i=0;i<4;i++){
        		int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        		Field[Fd(tY,tX)]=NextB[0].Blocktype;
        		if(tY>Highest) Highest=tY;
        		if(tY>0&&Bomb>0&&Field[Fd(tY-1,tX)]>=10) Bombx=tX;        		
			}
			if(Bombx>=0){
				for(int i=Bomb-1;i>=0;i--){///////clear bomb
					if(Field[Fd(i,Bombx)]>=10) {
						clrL(i); Bomb--;Bombclr++;
					}
					else break;
				}
			}
			for(int i=Highest;i>=0;i--){
				int sec=1;
        		for(int j=0;j<10;j++){
        			if(Field[Fd(i,j)]==0||Field[Fd(i,j)]==BlockType::W) {
        				sec=0;break;	
					}
				}
				if(sec){
					this->clrL(i); Lineclr++;
				}
			}
			
			/*
			計分方式（LINES SENT ,適用2P與6P）[2]
技巧名稱	Battle積分	Marathon積分
單列
Single	0 垃圾行線	100
雙列
Double	1 垃圾行線	300
三列
Triple	2 垃圾行線	500
四列
Tetris	4 垃圾行線	800
T旋轉
T-Spin (0 line cleared)	0 垃圾行線	100
T旋轉消除
T-Spin Mini	1 垃圾行線	300
T旋轉消除單列
T-Spin Single	2 垃圾行線	500
T旋轉消除雙列
T-Spin Double	4 垃圾行線	800
T旋轉消除三列
T-Spin Triple	6 垃圾行線	1200
B2B消除四列
B2B Tetris	6 垃圾行線	1200
B2B+T旋轉消除
B2B T-Spin Mini	2 垃圾行線	450
B2B+T旋轉消除單列
B2B T-Spin Single	3 垃圾行線	750
B2B+T旋轉消除雙列
B2B T-Spin Double	6 垃圾行線	1200
B2B+T旋轉消除三列
B2B T-Spin Triple	9 垃圾行線	1800
完美清除
Perfect Clear	10 垃圾行線	0
Combo積分
Combo次數	Battle積分
1-2	1 垃圾行線
3-4	2 垃圾行線
5-6	3 垃圾行線
7以上	4 垃圾行線
*/
			if(this->Highest==-1) {point+=10;PCclk.restart();PC=1;printf("PerfectClear!!! ");}//perfect clear
			if(Lineclr>0||Bombclr>0){
				if(playernum==2) printf("Player %d: ",Playername+1); 
				if(Playername==0){
					ClrM.stop();
					ClrM.play();	
				}
				if(Lineclr>=4||Tspin){ //B2B
					if(B2B){/////////////
						printf("B2B ");
						switch(Lineclr){//Tspin: Tspin=2  Tspin mini=1
						case 1: if(Tspin==1)point+=2; if(Tspin==2)point+=3;break;
						case 2: point+=6; break;							
						case 3: point+=9; break;
						case 4: point+=6; break;						
						}
					} else{
						switch(Lineclr){
						case 1: if(Tspin==1)point+=1; if(Tspin==2) point+=2;break;
						case 2: point+=4; break;							
						case 3: point+=6; break;
						case 4: point+=4; break;
						}
					}
					this->B2B=1;
				} else {
					switch(Lineclr){
						case 1: point+=0; break;
						case 2: point+=1; break;							
						case 3: point+=2; break;
						case 4: point+=4; break;
						}
					B2B=0; 
				}
				if(Tspin) printf("T-spin ");//
				switch(Lineclr){//print				
						case 1:
							switch(Tspin){
								case 0: 
								case 2:printf("Single "); break;
								case 1:printf("Mini "); break;
							} break;
						case 2: printf("Double "); break;							
						case 3: printf("Triple "); break;
						case 4: printf("Tetris "); break;
				}
				
				if(Bombclr) {
					printf("Bombclear*%d ",Bombclr);
					point+=Bombclr-1;
				}
				this->Combo++;
				if(Combo>=1){
					Comboclk.restart();
					switch(Combo){
						case 1:
						case 2:
							point+=1; break;
						case 3:
						case 4:
							point+=2; break;
						case 5:
						case 6:
							point+=3; break;
						default:
							point+=4;
					}
					printf("Combo:%d ",Combo);
					if(Combo<=7){
						ComboM[Combo%2].openFromFile("Combo/"+to_string(Combo)+".ogg");	
					}
					if(Combo==8){
						ComboM[0].openFromFile("Combo/7.ogg");	
					}
					if(Playername==0){
						ComboM[Combo%2].stop();
						ComboM[Combo%2].setVolume(100);
						ComboM[Combo%2].play();
					}
				}
				//point*=10+rand()%10;
				Linesent+=point;
				printf(" +%d\n",point);
				
			} else Combo=-1;
			if(BombOB){
				BombRB=BombOB;
				if(point>BombOB) {
					point-=BombOB; BombOB=0; 
				} else{
					BombOB-=point; point=0;
				} 
				addBomb(BombOB); BombOB=0;  Barclk.restart();
			}
			
			if(point&&playernum==2) Opponent->getBomb(point);
				printf("Player %d B2B:%d\n",Playername+1,B2B);
			RField();
			RLine();
			Next();
		}
		void RField(){//refresh field
			sf::RenderTexture RT;
			RT.create(230,463);
			RT.clear(sf::Color(0,0,0,0));
			
			for(int i=0;i<21;i++){
				for(int j=0;j<10;j++){
					if(Field[Fd(i,j)]!=0) {
						FieldS.setTexture(blkT[Field[Fd(i,j)]],true);
						FieldS.setPosition(23*j,440-23*i);
						RT.draw(FieldS);
					}					
				} 
			} 			
			RT.display();			
			FieldT=RT.getTexture();			
			FieldS.setTexture(FieldT,true);
			FieldS.setPosition(114+Playername*484,177);
		}
    	void RLine(){//refresh linesent
    		//printf("%d\n",this->Linesent);
    		sf::RenderTexture RT;
    		int width=digitof(this->Linesent),li=Linesent;
    		float resize=0.7;
			RT.create(34*width,38);
			RT.clear(sf::Color(0,0,0,0));
			LineS.setScale(1,1); LineS.setOrigin(0,0);
			for(int i=0;i<width;i++){
				LineS.setTexture(numT[li%10],true);
				LineS.setPosition((width-i-1)*34,0);
				RT.draw(LineS);
				li/=10;
			} 			
			RT.display();			
			LineT=RT.getTexture();			
			LineS.setTexture(LineT,true);
			LineS.setOrigin(17*width,19); LineS.setScale(resize,resize);
			LineS.setPosition(65+Playername*484,450);
		}
		void RCombo(){//refresh combo
			int milli=Comboclk.getElapsedTime().asMilliseconds();//1000ms
			if(Combo<=0) return;
			ComboT.loadFromFile("Combo/"+to_string(Combo)+".png");
			ComboS.setTexture(ComboT);
			ComboS.setPosition(46,161);
			ComboS.setColor(sf::Color(255,255,255,(int)(255.*(1000.-milli)/1000)));
		}
		void RPC(){//refresh perfect clear
			int milli=PCclk.getElapsedTime().asMilliseconds();//1000ms
			if(milli>3000) PC=0;
			if(PC<=0) return;
			PCT.loadFromFile("KO/Perfect.png");
			PCS.setTexture(PCT);
			PCS.setPosition(46,161);
			PCS.setColor(sf::Color(255,255,255,(int)(255.*(3000.-milli)/3000)));
		}
		void RKOed(){//refresh KOed			
			TimeT.loadFromFile("KO/KO.png");
			TimeS.setTexture(TimeT,true);
			TimeS.setPosition(46,207);
		}
		void RTimeup(){//
    		Refresh();
    		FdallS.setColor(sf::Color::White);
    		sf::RenderTexture RT;
    		RT.create(230,463);
    		RT.clear(sf::Color::Black);
    		FdallS.setPosition(0,0);
    		RT.draw(FdallS);
    		RT.display();
    		FdallT=RT.getTexture();
    		FdallS.setTexture(FdallT,true);
			FdallS.setColor(sf::Color(128,128,128,128));
			FdallS.setPosition(114+484*Playername,177);	
			TimeT.loadFromFile("KO/Time.png");
			TimeS.setTexture(TimeT,true);
			TimeS.setPosition(114+484*Playername,387);
		}
		void RTimeup2(){
			string s="win";			
			if(KO>Opponent->KO)s="win"; else if(KO<Opponent->KO) s="lose";
			else if(Linesent>Opponent->Linesent)s="win"; else if(Linesent<Opponent->Linesent) s="lose";
			else if(Highest<Opponent->Highest)s="win"; else if(Highest>Opponent->Highest) s="lose";	
			if(forceend){
				if(forceend-1==Playername) s="lose";
				else s="win";
			}		
			if(Playername==0){
				TimeM.openFromFile("KO/"+s+".ogg");
				TimeM.play();
			}
			TimeT.loadFromFile("KO/"+s+".png");
			TimeS.setTexture(TimeT,true);
			TimeS.setPosition(114+484*Playername,387);
		}
		void RBar(){
			float per=Barclk.getElapsedTime().asSeconds();
			if(per>1) per=1;
			sf::RenderTexture RT;
    		RT.create(8,463);
    		RT.clear();
    		sf::RectangleShape R;
    		sf::RectangleShape O;
    		R.setSize(sf::Vector2f(8,23*(BombRB+(BombOB-BombRB)*per)));
    		O.setSize(sf::Vector2f(8,23*BombOB));
    		R.setFillColor(sf::Color(197,32,21));
    		O.setFillColor(sf::Color(214,135,30));
    		R.setPosition(0,463-23*(BombRB+(BombOB-BombRB)*per));
    		O.setPosition(0,463-23*BombOB);
    		RT.draw(O); RT.draw(R);
    		RT.display();
    		BarT=RT.getTexture();
    		BarS.setTexture(BarT,true);			
			BarS.setPosition(349+484*Playername,177);
		}		
		
		void adjusty(){//for record mode to adjust the posc.y to make it reasonable
			int out=0;
			NextB[0].posc.y--;
			while(out==0){
				NextB[0].posc.y++;
        		for(int i=0;i<4;i++){
        			int tX=NextB[0].posc.x+NextB[0].pos[i].x,tY=NextB[0].posc.y+NextB[0].pos[i].y;
        			if(tY<0||Field[Fd(tY,tX)]!=0) {
        				break;
					}
					if(i==3)out=1;
				}				
			}			
		}
		bool JudgeDead(){
			int out=0;
    	    for(int i=0;i<4;i++){
    	    	int tY=NextB[0].posc.y+NextB[0].pos[i].y,tX=NextB[0].posc.x+NextB[0].pos[i].x;
        		if(Field[Fd(tY,tX)]!=0){
    	    		out=1; break;
				}
			}
			if(out){
				out=0; NextB[0].posc.y++;
				for(int i=0;i<4;i++){
      				int tY=NextB[0].posc.y+NextB[0].pos[i].y,tX=NextB[0].posc.x+NextB[0].pos[i].x;
     		   		if(Field[Fd(tY,tX)]!=0){
     		   		out=1; break;
					}
				}			
			}
			if(out){
				if(this->Bomb>0) KOreset();
				else forceend=1+Playername; //lose	
			}
		}
};

void TimeR(sf::Texture& T,sf::Sprite &S,sf::Clock &Clk){
	
	int time=Clk.getElapsedTime().asSeconds();
	if(time>120) time=120;
	sf::RenderTexture RT;
	RT.create(254,50);
	RT.clear(sf::Color(0,0,0,0));
	sf::RectangleShape rs;	
	rs.setSize(sf::Vector2f(126,5)); rs.setPosition(17,0);
	rs.setFillColor(sf::Color(80,50,14));//gray
	RT.draw(rs);	
	rs.setSize(sf::Vector2f(126*(120-time)/120,5)); rs.setPosition(17,0);
	rs.setFillColor(sf::Color(229,150,26));//yellow
	RT.draw(rs);
	
	
	
	S.setColor(sf::Color(255,255,255));	
	/*S.setTextureRect(sf::IntRect(17,0,126,5));
	S.setColor(sf::Color(80,50,14));//gray
	RT.draw(S);
	
	S.setTextureRect(sf::IntRect(17,0,126*(120-time)/120,5));
	S.setColor(sf::Color(229,150,26));//yellow
	RT.draw(S);
	
	S.setColor(sf::Color(255,255,255));*/
	
	S.setTexture(numT[(int)(120-time)/60/10],true);
	S.setPosition(0,12);
	RT.draw(S);
	
	S.setTexture(numT[(int)((120-time)/60)%10],true);
	S.setPosition(34,12);
	RT.draw(S);
	
	S.setTexture(numT[(int)((120-time)%60)/10],true);
	S.setPosition(86,12);
	RT.draw(S);
	
	S.setTexture(numT[(int)(120-time)%10],true);
	S.setPosition(120,12);
	RT.draw(S);
	
	RT.display();	
	T=RT.getTexture();	
	S.setTexture(T,true);
	S.setPosition(398,57);
}
/*
		J, L, S, T, Z Tetromino Wall Kick Data
Test 1	Test 2	Test 3	Test 4	Test 5
0>>1	( 0, 0)	(-1, 0)	(-1, 1)	( 0,-2)	(-1,-2)
1>>0	( 0, 0)	( 1, 0)	( 1,-1)	( 0, 2)	( 1, 2)
1>>2	( 0, 0)	( 1, 0)	( 1,-1)	( 0, 2)	( 1, 2)
2>>1	( 0, 0)	(-1, 0)	(-1, 1)	( 0,-2)	(-1,-2)
2>>3	( 0, 0)	( 1, 0)	( 1, 1)	( 0,-2)	( 1,-2)
3>>2	( 0, 0)	(-1, 0)	(-1,-1)	( 0, 2)	(-1, 2)
3>>0	( 0, 0)	(-1, 0)	(-1,-1)	( 0, 2)	(-1, 2)
0>>3	( 0, 0)	( 1, 0)	( 1, 1)	( 0,-2)	( 1,-2)

I Tetromino Wall Kick Data
Test 1	Test 2	Test 3	Test 4	Test 5
0>>1	( 0, 0)	(-2, 0)	( 1, 0)	(-2,-1)	( 1, 2)
1>>0	( 0, 0)	( 2, 0)	(-1, 0)	( 2, 1)	(-1,-2)
1>>2	( 0, 0)	(-1, 0)	( 2, 0)	(-1, 2)	( 2,-1)
2>>1	( 0, 0)	( 1, 0)	(-2, 0)	( 1,-2)	(-2, 1)
2>>3	( 0, 0)	( 2, 0)	(-1, 0)	( 2, 1)	(-1,-2)
3>>2	( 0, 0)	(-2, 0)	( 1, 0)	(-2,-1)	( 1, 2)
3>>0	( 0, 0)	( 1, 0)	(-2, 0)	( 1,-2)	(-2, 1)
0>>3	( 0, 0)	(-1, 0)	( 2, 0)	(-1, 2)	( 2,-1)
*/

int main() {
    srand(time(NULL));
    int tmpbomb=15;
    printf("Q: How many players do you need?(1 or 2)\nA: ");
    scanf("%d",&playernum);
    if(playernum>=2) playernum=2; else playernum=1;
    printf("Q: How many bombs do you need initially?(15 at most)\nA: ");
    scanf("%d",&tmpbomb);    
    if(playernum==1)printf("Get 40 points to win!!!\n");
    printf("Q: Load record?(1:yes 0:No)\nA: ");
    scanf("%d",&record);    
    if(record>0) record=1; else record=0;
    //printf("Loading...");
    PlayerInfo Player[2];
    
    KW3R[0][0].s(0,0); KW3R[0][1].s(-1,0); KW3R[0][2].s(-1,1); KW3R[0][3].s(0,-2); KW3R[0][4].s(-1,-2);
    KW3R[1][0].s(0,0); KW3R[1][1].s(1,0); KW3R[1][2].s(1,-1); KW3R[1][3].s(0,2); KW3R[1][4].s(1,2);
    KW3R[2][0].s(0,0); KW3R[2][1].s(1,0); KW3R[2][2].s(1,1); KW3R[2][3].s(0,-2); KW3R[2][4].s(1,-2);
    KW3R[3][0].s(0,0); KW3R[3][1].s(-1,0); KW3R[3][2].s(-1,-1); KW3R[3][3].s(0,2); KW3R[3][4].s(-1,2);
    
    KW4R[0][0].s(0,0); KW4R[0][1].s(-2,0); KW4R[0][2].s(1,0); KW4R[0][3].s(-2,-1); KW4R[0][4].s(1,2);
    KW4R[1][0].s(0,0); KW4R[1][1].s(-1,0); KW4R[1][2].s(2,0); KW4R[1][3].s(-1,2); KW4R[1][4].s(2,-1);
    KW4R[2][0].s(0,0); KW4R[2][1].s(2,0); KW4R[2][2].s(-1,0); KW4R[2][3].s(2,1); KW4R[2][4].s(-1,-2);
    KW4R[3][0].s(0,0); KW4R[3][1].s(1,0); KW4R[3][2].s(-2,0); KW4R[3][3].s(1,-2); KW4R[3][4].s(-2,1);
    
    int start=1,timeup=0;////////////////start should =1 start=0:game
    sf::Texture MainT,EffectT,TimeT;
    if(!MainT.loadFromFile("Main.png")) return 1;
    sf::Sprite MainS(MainT),EffectS,TimeS;
//MainM,DropM,ComboM,ClrM,HoldM,KOM,KOgetM
    if(!MainM.openFromFile("Start/Start.ogg")) return EXIT_FAILURE;
    if(!DropM.openFromFile("Node/Drop.ogg")) return EXIT_FAILURE;
    if(!ComboM[1].openFromFile("Combo/1.ogg")) return EXIT_FAILURE;
    if(!ComboM[0].openFromFile("Combo/2.ogg")) return EXIT_FAILURE;
    if(!ClrM.openFromFile("Node/Clear.ogg")) return EXIT_FAILURE;
    if(!HoldM.openFromFile("Node/Hold.ogg")) return EXIT_FAILURE;
    if(!KOM.openFromFile("KO/KO.ogg")) return EXIT_FAILURE;
    if(!KOgetM.openFromFile("KO/KOget.ogg")) return EXIT_FAILURE;
    if(!TimeM.openFromFile("KO/Time.ogg")) return EXIT_FAILURE;
    
    
    //sf::Clock MainC,inclk;
    sf::Sprite TestS; sf::Texture TestT;//////////////test
    for(int i=1;i<13;i++){
    	blkT[i].loadFromFile("Node/"+BlockTypeR(i)+".png");
	}
	for(int i=0;i<10;i++){
    	numT[i].loadFromFile("Number/"+to_string(i)+".png");
	}
	int recnum=-1;
    char recname1[25],recname2[25];
    if(record){
    	/*do{
			recnum++;
			string s="record"+to_string(recnum)+".txt";
			s.copy(recname1,s.size()+1);
			recin=fopen(recname1,"r");
			printf("Read from:%s",recname1);
		} while(recin!=NULL);*/
			recnum=1;
			string s="record"+to_string(recnum-1)+".txt";
			s.copy(recname1,s.size()+1);
			recin=fopen(recname1,"r");
			printf("Read from:%s",recname1);
		s="record"+to_string(recnum)+".txt";
		s.copy(recname2,s.size()+1);
		recout=fopen(recname2,"w");
	}		
	Player[0].reset(0,tmpbomb,&Player[1]); 
	if(playernum==1&&record==0)Player[1].reset(1,0,&Player[0]); else Player[1].reset(1,tmpbomb,&Player[0]);
    sf::RenderWindow window(sf::VideoMode(955,712),"Tetris Test");
    if(playernum==1) Player[1].Linesent=40;
    Player[0].RLine(); Player[1].RLine();
    Player[0].RField();  Player[1].RField();
    
	
    
    sf::Time refreshT=sf::milliseconds(100);
    //window.requestFocus();
    //if(window.hasFocus()==0)
	//refreshT=sf::milliseconds(50);if(!MainM.openFromFile("Main.ogg")) return EXIT_FAILURE;MainM.setVolume(50); MainM.play();///////////for test
    

    
    while(window.isOpen()){
        sf::Clock RefreshC;
        sf::Event event;
        window.clear();
     	window.draw(MainS);
     	
     	if((start==0&&MainC.getElapsedTime().asSeconds()>120)||forceend){//////time's up
     		
     	   	while(window.pollEvent(event)){
     	   		switch(event.type){
                case sf::Event::Closed://close window	
                    window.close(); break; 
  	          }
    	    }
    		    if(forceend<10&&forceend){
        			MainM.stop();
        			for(int i=0;i<playernum;i++){
        				Player[i].RTimeup();
        				Player[i].RTimeup2();
					}       			
        			forceend+=10; timeup=2;
				}
        		if(timeup==0){
        			TimeR(TimeT,TimeS,MainC);
        			TimeM.play();
        			for(int i=0;i<playernum;i++) Player[i].RTimeup();        			
        			timeup=1;
				}				
				if(timeup==1&&MainC.getElapsedTime().asSeconds()>123){
					timeup=2;
					for(int i=0;i<playernum;i++) Player[i].RTimeup2();
				}
				window.draw(TimeS);
				window.draw(Player[0].LineS);
				window.draw(Player[1].LineS);
				if(Player[0].KO) window.draw(Player[0].KOS);
				if(Player[1].KO) window.draw(Player[1].KOS);
				
				for(int j=0;j<playernum;j++){
					window.draw(Player[j].HoldB.blkS);
     			   window.draw(Player[j].FdallS);
     			   window.draw(Player[j].TimeS);
      			  for(int i=1;i<=5;i++){
      			  	window.draw(Player[j].NextB[i].blkS);
					}
				}
      		  window.display();
     		   sf::Time RefreshT=RefreshC.getElapsedTime();
     		   sf::sleep(refreshT-RefreshT);//
        		continue;        		
		}
		
			
		if(start){//start
			
			while(window.pollEvent(event)){
     	   		switch(event.type){
                case sf::Event::Closed://close window	
                    window.close(); break;				 
  	          }
    	    }
    	    
          	if(start==1){
        	        //MainM.play();
        	        MainC.restart();
        	        TimeR(TimeT,TimeS,MainC);
        	    if(window.hasFocus()){
				   start++; if(sf::Music::Stopped==MainM.getStatus()) MainM.play();
				} else MainM.stop();    
            } else start++;
     	  	EffectT.loadFromFile("Start/"+to_string(start-1)+".png");
     	  	EffectS.setTexture(EffectT,true);
      	    EffectS.setScale(952.d/600.,952.d/600.);
      	    EffectS.setPosition(1,137);
       	      
       	    if(start==65){
                start=0;
                MainC.restart();
                if(!MainM.openFromFile("Main.ogg")) return EXIT_FAILURE;
                 MainM.setVolume(50);
                 MainM.play();
                for(int i=0;i<playernum;i++){
                	Player[i].dwnclk.restart();
				}
                refreshT=sf::milliseconds(20);///
 //               window.setVerticalSyncEnabled(true);
    	    }
     	   
     	   window.draw(TimeS);
     	   window.draw(Player[0].LineS);
     	   window.draw(Player[1].LineS);
     	   window.draw(Player[0].FieldS);
     	   window.draw(Player[1].FieldS);
     	   window.draw(EffectS);
     	   window.display();
     	   sf::Time RefreshT=RefreshC.getElapsedTime();
     		   sf::sleep(refreshT-RefreshT);//
     	   continue;
      	}
        
        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed://close window	
                    window.close(); break;               
                case sf::Event::KeyPressed:
                	if(start) break;
                	if(playernum)
                   switch(event.key.code){
                       case sf::Keyboard::Up:
                       case sf::Keyboard::X:
                       	case sf::Keyboard::LControl:   
                           //turn0
                           	Player[0].turnR();
                           	break;
                       case sf::Keyboard::Z:
                           //turn 1;
    						Player[0].turnL();
                  	        break;
                       case sf::Keyboard::C:
                       case sf::Keyboard::LShift:
                           	//hold
                           		Player[0].Hold();//build-in check this->Holdyet
                           	break;
                       case sf::Keyboard::Space:
                       		DropM.stop();
                       		DropM.play();                       		
                       		Player[0].Drop();
                           	//drop
                           	break;
                           	
                        case sf::Keyboard::Numpad8://2 players
                           //turn0
                           if(playernum==2&&record==0)
                           	Player[1].turnR();
                           	break;
                       case sf::Keyboard::Numpad7:
                           //turn 1;
                           if(playernum==2&&record==0)
    						Player[1].turnL();
                  	        break;
                       	case sf::Keyboard::Numpad3:
                           	//hold
                           	if(playernum==2&&record==0)
                           	Player[1].Hold();//build-in check this->Holdyet
                           	break;
                       case sf::Keyboard::Numpad0:
                       		if(playernum==2&&record==0)
                       		Player[1].Drop();
                           	//drop
                           	break;
                       /*case sf::Keyboard::Down:
                        	Player[0].movD();
                        	//Player[0].dwnclk.restart();
                        	break;*/
                        /*case sf::Keyboard::Right:
                        	//if(++Player[0].NextB[0].posc.x>9) Player[0].NextB[0].posc.x=9; 
							Player[0].movR();
                        	break;
                        case sf::Keyboard::Left:                        	
                        	//if(--Player[0].NextB[0].posc.x<0) Player[0].NextB[0].posc.x=0; 
							Player[0].movL();
                        	break;*/
                   }
                	break;

            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){//dirDown  
        	if(Player[0].dwnP){
        		if(Player[0].dwnPclk.getElapsedTime()>sf::milliseconds(150)) Player[0].movD();        		
			}
        	else{
        		Player[0].dwnP=1; Player[0].dwnPclk.restart(); Player[0].movD();
			}
		}
		else{
			Player[0].dwnP=0;
		}
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){//dir  int movdir=-1;//0:right 1:left -1:none
        	if(Player[0].movdir==0){
        		if(Player[0].movclk.getElapsedTime()>sf::milliseconds(150)) Player[0].movR();
        		
			}
        	else{
        		Player[0].movdir=0; Player[0].movclk.restart(); Player[0].movR();
			}
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        	if(Player[0].movdir==1){
        		if(Player[0].movclk.getElapsedTime()>sf::milliseconds(150)) Player[0].movL();
        		
			}
        	else{
        		Player[0].movdir=1; Player[0].movclk.restart(); Player[0].movL();
			}
		}
		else{
			Player[0].movdir=-1;
		}
		if(Player[0].dwnclk.getElapsedTime()>sf::milliseconds(1000)){
        	Player[0].movD();
		}
		
	if(playernum==2&&record==0){
        
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5)){//dirDown  
        	if(Player[1].dwnP){
        		if(Player[1].dwnPclk.getElapsedTime()>sf::milliseconds(150)) Player[1].movD();        		
			}
        	else{
        		Player[1].dwnP=1; Player[1].dwnPclk.restart(); Player[1].movD();
			}
		}
		else{
			Player[1].dwnP=0;
		}
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)){//dir  int movdir=-1;//0:right 1:left -1:none
        	if(Player[1].movdir==0){
        		if(Player[1].movclk.getElapsedTime()>sf::milliseconds(150)) Player[1].movR();
        		
			}
        	else{
        		Player[1].movdir=0; Player[1].movclk.restart(); Player[1].movR();
			}
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)){
        	if(Player[1].movdir==1){
        		if(Player[1].movclk.getElapsedTime()>sf::milliseconds(150)) Player[1].movL();
        		
			}
        	else{
        		Player[1].movdir=1; Player[1].movclk.restart(); Player[1].movL();
			}
		}
		else{
			Player[1].movdir=-1;
		}		
        if(Player[1].dwnclk.getElapsedTime()>sf::milliseconds(1000)){
        	Player[1].movD();
		}
	}
		//////////////////////////////////////////////////////////////////////////
		if(record&&playernum==2){
			int timb=MainC.getElapsedTime().asMilliseconds();
			int c;
			while(Player[1].rectim+Player[1].KOt*1000<timb&&Player[1].rectim>0){
				fscanf(recin,"%d",&c); 
				if(fscanf(recin,"%c",&c)!=1) {
					Player[1].rectim=-1; break;
				}
				else printf(" %c",c);/////
				int tx,ty;
				switch((char)c){
					case 'm':						
						fscanf(recin,"%d%d",&tx,&ty); printf(" %d %d",tx,ty);///// 
						//if((ty+=Player[1].Bomb)>19) ty=19;
						Player[1].NextB[0].posc.x=tx; Player[1].NextB[0].posc.y=ty+Player[1].Bomb;
						Player[1].adjusty();	
						if(fscanf(recin,"%d",&Player[1].rectim)!=1) Player[1].rectim=-1; printf("\n%d",Player[1].rectim);///// 
						break;
					case 'd':
						Player[1].Drop();
						if(fscanf(recin,"%d",&Player[1].rectim)!=1) Player[1].rectim=-1;
						break;
					case 'r':
						fscanf(recin,"%d",&c); printf(" %d",c);/////
						Player[1].NextB[0].reset(Player[1].NextB[0].Blocktype,c);
						if(fscanf(recin,"%d",&c)) {
							Player[1].rectim=c; printf("\n%d",c);/////
							break;
						}
						else {
							if(fscanf(recin,"%c",&c)!=1) Player[1].rectim=-1;
							else printf(" %c",c);/////
							fscanf(recin,"%d%d",&tx,&ty); printf(" %d %d",tx,ty);///// 
							Player[1].NextB[0].posc.x=tx; Player[1].NextB[0].posc.y=ty+Player[1].Bomb;
							if(fscanf(recin,"%d",&Player[1].rectim)!=1) Player[1].rectim=-1;
						}
						
						break;
					case 'h':
						Player[1].Hold();
						if(fscanf(recin,"%d",&Player[1].rectim)!=1) Player[1].rectim=-1;
						break;		
					default:
					    printf(" %d Char error!!!\n",c);			   
				}
					
			}
					
		}
		TimeR(TimeT,TimeS,MainC);
           
        

            	//window.draw(TestS);
        //Player[0].NextB[0].blkS.setTexture(blkT[2],true);//NextB[0].T
		//Player[0].NextB[0].blkS.setPosition(200,200);///////////////
		/*if(Player[0].NextB[0].posc.y==19){
			Player[0].NextB[0].
		}*/
		for(int j=0;j<playernum;j++){
			if(Player[j].KOed==0)Player[j].Refresh();
			window.draw(Player[j].HoldB.blkS);
     	   window.draw(Player[j].FdallS);
     	   for(int i=1;i<=5;i++){
        	window.draw(Player[j].NextB[i].blkS);
			}
			if(Player[j].BombRB||Player[j].BombOB){
				Player[j].RBar();
				window.draw(Player[j].BarS);
			}
			/////
			if(Player[j].KOed&&Player[j].KOclk.getElapsedTime().asSeconds()<1.){//////KOed
			if(Player[j].KOed==1) {
				Player[j].RKOed(); Player[j].KOed=2;Player[j].Refresh();
			}
			//window.draw(Player[j].DarkS); window.draw(Player[j].TimeS);
		}else if(Player[j].KOed){
			Player[j].KOed=0; Player[j].FdallS.setColor(sf::Color::White); Player[j].RField(); Player[j].dwnclk.restart();	
			Player[j].JudgeDead();
		}///////
		}				
		window.draw(TimeS);
		window.draw(Player[0].LineS);
		window.draw(Player[1].LineS);		
		if(Player[1].KO) window.draw(Player[1].KOS);        
		if(Player[0].KO) window.draw(Player[0].KOS);
        //window.draw(Player[0].NextB[0].blkS);
        
        window.display();
        sf::Time RefreshT=RefreshC.getElapsedTime();
        if((refreshT-RefreshT).asMilliseconds()>0)sf::sleep(refreshT-RefreshT);//
        
    }
    fclose(recout); fclose(recin); 
    MainM.stop();
    system("pause");
    return EXIT_SUCCESS;
}

