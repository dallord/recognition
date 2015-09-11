#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include "learning.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

LeptonThread::LeptonThread() : QThread()
{
}

LeptonThread::~LeptonThread() {
}


void LeptonThread::run()
{
    //create the initial image
    myImage = QImage(80, 60, QImage::Format_RGB888);

    //open spi port
    SpiOpenPort(0);

    ofstream out("../data/outData.txt");

    ifstream infiletrain("../../data/training_set.txt"); //open file with training set

    ofstream outfile("../../data/result.txt"); //output file

    int counter = 0;
    X u; //learning point

    //count number of training vectors
    string buff;
    counter_train = 0;
    while (!infiletrain.eof()){
        getline(infiletrain, buff);
        counter_train++;
    }
    infiletrain.close();

    counter_train--;

    //init sets and coefficients
    training_in = new X[counter_train];
    training_out = new Y[counter_train];

    gam = new float[counter_train];
    init_gamma();
    h = 5; //can be other

    init_classes();

    //filling training set
    infiletrain.open("../../data/training_set.txt");
    string line;
    int i = 0;
    while(!infiletrain.eof()){
        getline(infiletrain, line);
        sscanf(line.c_str(), "%d %d %f %f %f",
               &training_in[i].x, &training_in[i].y,
               &training_out[i].s0, &training_out[i].s1, &training_out[i].s2);
        set_type(training_out[i]);
        i++;
    }
    infiletrain.close();

   // test();

    while(true) {

        //read data packets from lepton over SPI
        int resets = 0;
        for(int j=0;j<PACKETS_PER_FRAME;j++) {
            //if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
            read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
            int packetNumber = result[j*PACKET_SIZE+1];
            if(packetNumber != j) {
                j = -1;
                resets += 1;
                usleep(1000);
                //Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
                //By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
                if(resets == 750) {
                    SpiClosePort(0);
                    usleep(750000);
                    SpiOpenPort(0);
                }
            }
        }
        if(resets >= 30) {
            qDebug() << "done reading, resets: " << resets;
        }

        frameBuffer = (uint16_t *)result;
        int row, column;
        uint16_t value;
        uint16_t minValue = 65535;
        uint16_t maxValue = 0;

        char buf[5];
        std::vector<int> line; //copy of frameBuffer to line


        for(int i=0;i<FRAME_SIZE_UINT16;i++) {
            //skip the first 2 uint16_t's of every packet, they're 4 header bytes
            if(i % PACKET_SIZE_UINT16 < 2) {
                continue;
            }

            //flip the MSB and LSB at the last second
            int temp = result[i*2];
            result[i*2] = result[i*2+1];
            result[i*2+1] = temp;

            value = frameBuffer[i];
            if(value > maxValue) {
                maxValue = value;
            }
            if(value < minValue) {
                minValue = value;
            }
            column = i % PACKET_SIZE_UINT16 - 2;
            row = i / PACKET_SIZE_UINT16 ;

            if (counter%27 == 0){
                sprintf(buf, "%u", frameBuffer[i]);
                out << buf << " ";

                line.push_back((int)frameBuffer[i]);
            }
        }

        if (counter%27 == 0){ //handling the line
            out << "\n";

            u = find_ref_point(line);

            learning_in = u;

            string classname;
            int A;

            A = algorithm(learning_in);
            if (A == 0) classname = "no object";
            if (A == 1) classname = "standing";
            if (A == 2) classname = "sitting";
            if (A == 3) classname = "lying";
            outfile << learning_in.x << " " << learning_in.y << " = " << classname << endl;
            outfile.close();

            cout << learning_in.x << " " << learning_in.y << " = " << classname << endl;

        }


        //qDebug() << minValue <<  " "  << maxValue;
        //minValue = 6000;
        //maxValue = 9000;

        float diff;
        if (minValue < 7860)
            diff = 402;//maxValue - minValue;
        else diff = 300;

        float scale = 255/diff;
        QRgb color;
        for(int i=0;i<FRAME_SIZE_UINT16;i++) {
            if(i % PACKET_SIZE_UINT16 < 2) {
                continue;
            }
            value = (frameBuffer[i] - minValue) * scale;
            const int *colormap = colormap_rainbow;
            color = qRgb(colormap[3*value], colormap[3*value+1], colormap[3*value+2]);
            column = (i % PACKET_SIZE_UINT16 ) - 2;
            row = i / PACKET_SIZE_UINT16;
            myImage.setPixel(column, row, color);
        }

        //lets emit the signal for update
        emit updateImage(myImage);
        counter++;


    }
    out.close();

    //finally, close SPI port just bcuz
    SpiClosePort(0);
}

void LeptonThread::performFFC() {
    //perform FFC
    lepton_perform_ffc();
}

void LeptonThread::Test(){
    test();
}
