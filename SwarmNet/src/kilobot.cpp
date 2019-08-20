#pragma once
#include "../../../src/kilolib.h"
#include "swarmnet.h"
#include <math.h>


class mykilobot : public kilobot
{

/*----------------------------------------------------------------------*/
/*-----------------------------           ------------------------------*/
/*----------------------------- User code ------------------------------*/
/*-----------------------------           ------------------------------*/
/*----------------------------------------------------------------------*/

    message_t message;
	Swarmnet s;
    Channel *c;
    Publisher p;
    Subscriber sub; 

    unsigned char msg1[10];
    int ret;
    bool go_away;
    int go_away_turn;
    int go_away_time;
    int turn_direction;
    int num_times;

    int send;

    void sent() {
        if (send > 0 && id == 3) {
            p.send(msg1, strlen((char*)msg1));
            send = send - 1;
            printf("SENT!! ");
            printf("%d\r\n", send);
        }
    }

    // call_back example    
    void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
        if (msg[3] == 3){
            set_color(RGB(0,1,0));
            num_times++;
            printf("%d\r\n", num_times);
        }
        // int distance = meta->dist;
        // float angle = meta->theta;
        // if (id == 1 && distance < 40){
        //     go_away = true;
        //     go_away_time = 100;
        //     go_away_turn = rand() % 31;
        //     turn_direction = rand() % 2;
        // }
        // else if (id == 2 && distance < 200){
        //     go_away = true;
        //     go_away_time = 150;
        //     go_away_turn = rand() % 31;
        //     turn_direction = rand() % 2;
        // }
    } 
    

	void loop()
	{
        msg1[3] = id;
        if (id == 3) set_color(RGB(1,1,1));
        // if (id == 1) set_color(RGB(0,1,0));
        // else if (id == 2) set_color(RGB(1,0,1));
        // else {
        //     set_color(RGB(1,1,1));
        //     strcpy((char*)msg1, "specialmsg");
        // }
        // if (go_away == true){
        //     if (go_away_turn == 0){
        //         go_away_time = go_away_time - 1;
        //         set_motors(50,50);
        //         spinup_motors();
        //     }
        //     else {
        //         go_away_turn = go_away_turn - 1;
        //         if (turn_direction == 0){
        //             set_motors(50, 0);
        //             spinup_motors();
        //         }
        //         else {
        //             set_motors(0, 50);
        //             spinup_motors();
        //         }
        //     }

        //     if (go_away_time == 0){
        //         go_away = false;
        //     }

        // }
        // else {    
        //     if (id == 1) set_color(RGB(0,1,0));
        //     else set_color(RGB(1,0,1));

        //     if (abs(angle_to_light) < 0.05) {
        //             set_motors(50,50);
        //             spinup_motors();
        //         }
        //     else {
        //             set_motors(50,0);
        //             spinup_motors();
        //         }
        //     }
	}

	//executed once at start
	void setup()
	{
        strcpy((char*)msg1, "aaaaaaaaaa");
        set_color(RGB(1,0,0));

        c = s.new_channel(2, 2, false);
        p.init(c, std::bind(&mykilobot::sent, this));
        sub.init(100, std::bind(&mykilobot::call_back, this, 
                                 std::placeholders::_1, 
                                 std::placeholders::_2, 
                                 std::placeholders::_3,
                                 std::placeholders::_4));

        c->subscribe(&sub);

        send = 50;
        
        p.send(msg1, strlen((char*)msg1));
        message.type = NORMAL;
        ret = s.next_pkt(message.data);
        message.crc = message_crc(&message);
        go_away = false;
        num_times = 0;
	}

    
/*----------------------------------------------------------------------*/
/*-----------------------------             ----------------------------*/
/*----------------------------- Driver code ----------------------------*/
/*-----------------------------             ----------------------------*/
/*----------------------------------------------------------------------*/
    
    //executed on successfull message send
    void message_tx_success()
    {

    }

    //sends message at fixed rate
    message_t *message_tx()
    {
        message.type = NORMAL;
        int ret = s.next_pkt(message.data);
        message.crc = message_crc(&message);

        if(ret == 0) return NULL;
        static int r = rand() % 100;
        if(r <= 90) return &message;
        return NULL;
    }

    void message_rx(message_t *message, distance_measurement_t *distance_measurement,float t)
    {
        int dist = estimate_distance(distance_measurement);
    	Meta_t meta;
    	meta.dist = dist;
    	meta.theta = t;
    	s.receive(message->data, PKT_SIZE, &meta);
    }
};
