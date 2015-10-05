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

    ofstream outfile("../data/result.txt"); //output file

    int counter = 0;

    X u; //learning points

    //count number of training vectors
    ifstream infiletrain("../data/experiments/training_set.txt"); //open file with training set
    string buff;
    counter_train = 0;
    while (!infiletrain.eof()){
        getline(infiletrain, buff);
        counter_train++;
    }
    infiletrain.close();

    counter_train--;

    //init sets and coefficients
    //counter_train = 140;
    training_set = new Pair[counter_train];

    gam = new float[counter_train];    

    init_classes();


    //filling training set
    infiletrain.open("../data/experiments/training_set.txt");
    string line;
    int i = 0;
    while(!infiletrain.eof()){
        getline(infiletrain, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &training_set[i].training_in.x0, &training_set[i].training_in.y0, &training_set[i].training_in.x1, &training_set[i].training_in.y1, &training_set[i].training_in.x2, &training_set[i].training_in.y2,
               &training_set[i].training_out.s0, &training_set[i].training_out.s1, &training_set[i].training_out.s2, &training_set[i].training_out.s3);
        training_set[i].training_out.type = set_type(training_set[i].training_out);
        i++;
    }
    infiletrain.close();

    h = 3; //can be other
    init_gamma();

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

        //if (counter%27 == 0)  out << "sec " << counter/27 <<": ";


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

            u = find_ref_points(line);

            learning_in = u;

            string classname;
            Y ans;
            int A;


            ans = algorithm(learning_in);
            A = ans.type;
            if (A == 0) classname = "no object";
            if (A == 1) classname = "standing";
            if (A == 2) classname = "sitting";
            if (A == 3) classname = "lying on the bed";
            if (A == 4) classname = "lying on the floor";
            outfile << learning_in.x0 << " " << learning_in.y0 << " "
                    << learning_in.x1 << " " << learning_in.y1 << " "
                    << learning_in.x2 << " " << learning_in.y2 <<
                       " = " << ans.s0 << " " << ans.s1 << " " << ans.s2 << " " << ans.s3 << endl;


            cout << learning_in.x0 << " " << learning_in.y0 << " "
                 << learning_in.x1 << " " << learning_in.y1 << " "
                 << learning_in.x2 << " " << learning_in.y2 <<
                    " = "  << ans.s0 << " " << ans.s1 << " " << ans.s2 << " " << ans.s3 << " - "
                    << classname << endl;

        }


        //cout << minValue <<  " "  << maxValue << endl;

        //---------------------------
        //temperature correlation
        //---------------------------
        //7900-8000 - environment
        //8100-8300 - body
        //>8400 - hot objects
        //---------------------------

        float diff;
        if (minValue > 7900)
            minValue = 7900;
        if (maxValue < 8300)
            maxValue = 8300;
        diff = maxValue - minValue;

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
            if ((column == u.x0) && (row == u.y0)) color = qRgb(0, 0, 0);
            if ((column == u.x1) && (row == u.y1)) color = qRgb(0, 0, 0);
            if ((column == u.x2) && (row == u.y2)) color = qRgb(0, 0, 0);
            myImage.setPixel(column, row, color);
        }

        //lets emit the signal for update
        emit updateImage(myImage);
        counter++;
        line.clear();


    }
    out.close();
    outfile.close();

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
