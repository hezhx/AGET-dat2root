// AGET v3 decode
// dat to root and txt
// written by He Zhixuan 
// Pls contact hezhx21@lzu.edu.cn whenever you have a doubt
// AGET v3 data structure : 
// https://tesnus.site/2021/11/22/AGET%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84/

#include <string>
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <numeric>
#include <vector>

#include "user_defined_func.h"

using namespace std;

int Dat2Root(const char file[]){
    gROOT->Reset();

    // open the dat(hex data file)
    char filename[256] = "";
    memset(filename, 0, sizeof(filename));
    strcpy(filename, file);

    FILE *fid = fopen(filename, "rb");
    if (fid == NULL){
        printf("Open file : %s failed.\n", filename);
        exit(1);
    }
    else{
        printf("File : %s opened.\n", filename);
    }

    fstream dat_file;
    dat_file.open(filename, ios::binary|ios::in);
    if(!dat_file) {
        cout << "Error : ";
        return 0;
    }

    dat_file.seekg(0, ios_base::beg);   // move to the begin of the file
    ifstream::pos_type begin_pos = dat_file.tellg();
    dat_file.seekg(0, ios_base::end);   // move to the end of the file
    ifstream::pos_type end_pos = dat_file.tellg();
    auto file_size = (size_t)(end_pos-begin_pos);;
    cout << "File size : " << dec << file_size << endl;

    dat_file.close();

    unsigned short* file_origin_packet = new unsigned short[file_size];
    auto file_data = fread(file_origin_packet, sizeof(unsigned short), file_size, fid); //?
    cout << "File data : " << dec << file_data << endl;
    
    unsigned short* Packet = new unsigned short[file_data];
    for (int i = 0; i < file_data; i++)
    {
        Packet[i] = high_low_exchange(file_origin_packet[i]);
    }
    fclose(fid);

    // .dat file was saved to a array Packet[] with a 16-bit unit
    // for (int i = 0; i < 10; i++)
    // {
    //     cout << "Packet " << i << " : " << hex << Packet[i] << "\n";
    // }

    unsigned short START_OF_PACKET = 0xac0f;       // 1010 1100 0000 1111
    unsigned short take_SOE_bit = 0x4000;          // 0100 0000 0000 0000
    unsigned short take_EOE_bit = 0x2000;          // 0010 0000 0000 0000
    unsigned short take_Packet_Size_bits = 0x1fff; // 0001 1111 1111 1111
    unsigned short take_SOURCE_ID_bits = 0x001f;   // 0000 0000 0001 1111
    unsigned short take_Card_num_bits = 0x3e00;    // 0011 1110 0000 0000
    unsigned short take_Chip_num_bits = 0x0180;    // 0000 0001 1000 0000
    unsigned short take_Chn_num_bits = 0x007f;     // 0000 0000 0111 1111
    unsigned short take_ADC_code_bits = 0x0fff;    // 0000 1111 1111 1111

    int source_ID = 0;
    unsigned timestamp = 0;
    int event_ID = 0;
    int Card_num = 0;
    int Chip_num = 0;
    int Chn_num = 0;
    int Data_num = 0;
    unsigned int current_position = 0;
    int current_packet_length = 0;

    int total_event = 0.;

    double ADC_code[513] = {0.};

    Double_t baseline1 = 0.;
    Double_t baseline2 = 0.;
    Double_t baseline0 = 0.;

    Int_t event;
    Int_t time;
    Int_t board;
    Int_t chip;
    Int_t channel;
    Double_t amplitude;
    Double_t baseline;
    Double_t adc[512];

    TFile *f1 = new TFile(strcat(filename, ".root"), "RECREATE");
    strcpy(filename, file);
	TTree *T = new TTree("T", "AGET Dat");
    cout << strcat(filename, ".root") << " was created.\n";
    cout << "'T' tree was created.\n";
    strcpy(filename, file);

	T->Branch("event", &event, "event/I");
	T->Branch("time", &time, "time/I");	
	T->Branch("board", &board, "board/I");	
	T->Branch("chip", &chip, "chip/I");	
	T->Branch("channel", &channel, "channel/I");	
	T->Branch("amplitude", &amplitude, "amplitude/D");
    T->Branch("baseline", &baseline, "baseline/D");	
	T->Branch("adc", adc, "adc[512]/D");	
    
    // if (START_OF_PACKET == Packet[0]){
    //     cout << "Same\n";
    // }
    // cout << START_OF_PACKET << endl;

    // ofstream xinhao;
    // xinhao.open(strcat(filename, ".txt"));
    // strcpy(filename, file);
    // cout << strcat(filename, ".txt") << " was created.\nWaiting...\n";
    // strcpy(filename, file);

    for (int i = 0; i < (file_data - 1024); i++){
    // for (int i = 0; i < 600; i++){

        // if(Packet[i] == START_OF_PACKET &&
        //    ((current_position == 0 && ((Packet[i + 1] & take_SOE_bit) != 0)) || i == current_position + current_packet_length)){
        if (Packet[i] == START_OF_PACKET &&
            ((current_position == 0 && ((Packet[i + 1] & take_SOE_bit) != 0)) || i == current_position + current_packet_length)){
            // cout << i << endl;
            current_position = i;
            current_packet_length = (Packet[i + 1] & take_Packet_Size_bits) / 2 + 4;
            // cout << current_position << " ~ " << current_packet_length << endl;

            if((Packet[i+1] & take_SOE_bit) != 0){
                // current_packet_length = (Packet[i + 1] & take_Packet_Size_bits) / 2 + 4;
                // cout << current_packet_length << endl;
                source_ID = (Packet[i + 2] & take_SOURCE_ID_bits);
                timestamp = Packet[i + 4] * pow(2., 16) + Packet[i + 5] * pow(2., 32) + Packet[i + 3];
                event_ID = Packet[i + 6] + Packet[i + 7] * pow(2, 16);
                // cout << "Im here1.\n";
            }
            // cout << "Im here2.\n";

            if(((Packet[i+1] & take_SOE_bit) == 0) && ((Packet[i+1] & take_EOE_bit) == 0)){
                current_packet_length = (Packet[i + 1] & take_Packet_Size_bits) / 2 + 4;
                // from one doe's 2nd packet, acquire card (board), chip, channel 
                Card_num = (Packet[i + 2] & take_Card_num_bits) * pow(2, -9);
                Chip_num = (Packet[i + 2] & take_Chip_num_bits) * pow(2, -7);
                Chn_num = Packet[i + 2] & take_Chn_num_bits;
                Data_num = 1;

                // from one doe's 3rd packet, acquire adc code
                // for (int k = 1; k <= 512; k++){
                for (int j = i + 3; j <= (i + current_packet_length - 4); j++){
                    ADC_code[Data_num] = (Packet[j] & take_ADC_code_bits); 
                    Data_num = Data_num + 1;
                    // cout << (Packet[j] & take_ADC_code_bits) << endl;
                }
                // }
                // cout << ADC_code[1] << " " << ADC_code[256] << endl;

                total_event = total_event + 1;
                
                baseline1 = accumulate(ADC_code, ADC_code+30, 0);
                baseline2 = accumulate(ADC_code+412, ADC_code+512, 0);
                baseline0 = (baseline1 + baseline2)/130.;

                event = event_ID;
                time = timestamp;
                board = Card_num;
                chip = Chip_num;
                channel = Chn_num;
                amplitude = *max_element(ADC_code, ADC_code+512) - baseline0;   // maxium
                baseline = baseline0;

                for (int d = 0; d < 512; d++){
                    adc[d] = ADC_code[d+1];
                }
                T->Fill();

                // save a txt for check, annotate if unnecessary
                // xinhao  << event_ID << " " << timestamp << " " //<< source_ID << " "
                //         << Card_num << " " << Chip_num << " " << Chn_num << " " 
                //         << amplitude << " " << baseline << " "
                //         << ADC_code[1] << " " << ADC_code[2] << " " << ADC_code[3] << " " << ADC_code[4] << " " << ADC_code[5] << " " << ADC_code[6] << " " << ADC_code[7] << " " << ADC_code[8] << " " << ADC_code[9] << " " << ADC_code[10] << " " << ADC_code[11] << " " << ADC_code[12] << " " << ADC_code[13] << " " << ADC_code[14] << " " << ADC_code[15] << " " << ADC_code[16] << " " << ADC_code[17] << " " << ADC_code[18] << " " << ADC_code[19] << " " << ADC_code[20] << " " << ADC_code[21] << " " << ADC_code[22] << " " << ADC_code[23] << " " << ADC_code[24] << " " << ADC_code[25] << " " << ADC_code[26] << " " << ADC_code[27] << " " << ADC_code[28] << " " << ADC_code[29] << " " << ADC_code[30] << " " << ADC_code[31] << " " << ADC_code[32] << " " << ADC_code[33] << " " << ADC_code[34] << " " << ADC_code[35] << " " << ADC_code[36] << " " << ADC_code[37] << " " << ADC_code[38] << " " << ADC_code[39] << " " << ADC_code[40] << " " << ADC_code[41] << " " << ADC_code[42] << " " << ADC_code[43] << " " << ADC_code[44] << " " << ADC_code[45] << " " << ADC_code[46] << " " << ADC_code[47] << " " << ADC_code[48] << " " << ADC_code[49] << " " << ADC_code[50] << " " << ADC_code[51] << " " << ADC_code[52] << " " << ADC_code[53] << " " << ADC_code[54] << " " << ADC_code[55] << " " << ADC_code[56] << " " << ADC_code[57] << " " << ADC_code[58] << " " << ADC_code[59] << " " << ADC_code[60] << " " << ADC_code[61] << " " << ADC_code[62] << " " << ADC_code[63] << " " << ADC_code[64] << " " 
                //         << ADC_code[65] << " " << ADC_code[66] << " " << ADC_code[67] << " " << ADC_code[68] << " " << ADC_code[69] << " " << ADC_code[70] << " " << ADC_code[71] << " " << ADC_code[72] << " " << ADC_code[73] << " " << ADC_code[74] << " " << ADC_code[75] << " " << ADC_code[76] << " " << ADC_code[77] << " " << ADC_code[78] << " " << ADC_code[79] << " " << ADC_code[80] << " " << ADC_code[81] << " " << ADC_code[82] << " " << ADC_code[83] << " " << ADC_code[84] << " " << ADC_code[85] << " " << ADC_code[86] << " " << ADC_code[87] << " " << ADC_code[88] << " " << ADC_code[89] << " " << ADC_code[90] << " " << ADC_code[91] << " " << ADC_code[92] << " " << ADC_code[93] << " " << ADC_code[94] << " " << ADC_code[95] << " " << ADC_code[96] << " " << ADC_code[97] << " " << ADC_code[98] << " " << ADC_code[99] << " " << ADC_code[100] << " " << ADC_code[101] << " " << ADC_code[102] << " " << ADC_code[103] << " " << ADC_code[104] << " " << ADC_code[105] << " " << ADC_code[106] << " " << ADC_code[107] << " " << ADC_code[108] << " " << ADC_code[109] << " " << ADC_code[110] << " " << ADC_code[111] << " " << ADC_code[112] << " " << ADC_code[113] << " " << ADC_code[114] << " " << ADC_code[115] << " " << ADC_code[116] << " " << ADC_code[117] << " " << ADC_code[118] << " " << ADC_code[119] << " " << ADC_code[120] << " " << ADC_code[121] << " " << ADC_code[122] << " " << ADC_code[123] << " " << ADC_code[124] << " " << ADC_code[125] << " " << ADC_code[126] << " " << ADC_code[127] << " " << ADC_code[128] << " " 
                //         << ADC_code[129] << " " << ADC_code[130] << " " << ADC_code[131] << " " << ADC_code[132] << " " << ADC_code[133] << " " << ADC_code[134] << " " << ADC_code[135] << " " << ADC_code[136] << " " << ADC_code[137] << " " << ADC_code[138] << " " << ADC_code[139] << " " << ADC_code[140] << " " << ADC_code[141] << " " << ADC_code[142] << " " << ADC_code[143] << " " << ADC_code[144] << " " << ADC_code[145] << " " << ADC_code[146] << " " << ADC_code[147] << " " << ADC_code[148] << " " << ADC_code[149] << " " << ADC_code[150] << " " << ADC_code[151] << " " << ADC_code[152] << " " << ADC_code[153] << " " << ADC_code[154] << " " << ADC_code[155] << " " << ADC_code[156] << " " << ADC_code[157] << " " << ADC_code[158] << " " << ADC_code[159] << " " << ADC_code[160] << " " << ADC_code[161] << " " << ADC_code[162] << " " << ADC_code[163] << " " << ADC_code[164] << " " << ADC_code[165] << " " << ADC_code[166] << " " << ADC_code[167] << " " << ADC_code[168] << " " << ADC_code[169] << " " << ADC_code[170] << " " << ADC_code[171] << " " << ADC_code[172] << " " << ADC_code[173] << " " << ADC_code[174] << " " << ADC_code[175] << " " << ADC_code[176] << " " << ADC_code[177] << " " << ADC_code[178] << " " << ADC_code[179] << " " << ADC_code[180] << " " << ADC_code[181] << " " << ADC_code[182] << " " << ADC_code[183] << " " << ADC_code[184] << " " << ADC_code[185] << " " << ADC_code[186] << " " << ADC_code[187] << " " << ADC_code[188] << " " << ADC_code[189] << " " << ADC_code[190] << " " << ADC_code[191] << " " << ADC_code[192] << " " 
                //         << ADC_code[193] << " " << ADC_code[194] << " " << ADC_code[195] << " " << ADC_code[196] << " " << ADC_code[197] << " " << ADC_code[198] << " " << ADC_code[199] << " " << ADC_code[200] << " " << ADC_code[201] << " " << ADC_code[202] << " " << ADC_code[203] << " " << ADC_code[204] << " " << ADC_code[205] << " " << ADC_code[206] << " " << ADC_code[207] << " " << ADC_code[208] << " " << ADC_code[209] << " " << ADC_code[210] << " " << ADC_code[211] << " " << ADC_code[212] << " " << ADC_code[213] << " " << ADC_code[214] << " " << ADC_code[215] << " " << ADC_code[216] << " " << ADC_code[217] << " " << ADC_code[218] << " " << ADC_code[219] << " " << ADC_code[220] << " " << ADC_code[221] << " " << ADC_code[222] << " " << ADC_code[223] << " " << ADC_code[224] << " " << ADC_code[225] << " " << ADC_code[226] << " " << ADC_code[227] << " " << ADC_code[228] << " " << ADC_code[229] << " " << ADC_code[230] << " " << ADC_code[231] << " " << ADC_code[232] << " " << ADC_code[233] << " " << ADC_code[234] << " " << ADC_code[235] << " " << ADC_code[236] << " " << ADC_code[237] << " " << ADC_code[238] << " " << ADC_code[239] << " " << ADC_code[240] << " " << ADC_code[241] << " " << ADC_code[242] << " " << ADC_code[243] << " " << ADC_code[244] << " " << ADC_code[245] << " " << ADC_code[246] << " " << ADC_code[247] << " " << ADC_code[248] << " " << ADC_code[249] << " " << ADC_code[250] << " " << ADC_code[251] << " " << ADC_code[252] << " " << ADC_code[253] << " " << ADC_code[254] << " " << ADC_code[255] << " " << ADC_code[256] << " " 
                //         << ADC_code[257] << " " << ADC_code[258] << " " << ADC_code[259] << " " << ADC_code[260] << " " << ADC_code[261] << " " << ADC_code[262] << " " << ADC_code[263] << " " << ADC_code[264] << " " << ADC_code[265] << " " << ADC_code[266] << " " << ADC_code[267] << " " << ADC_code[268] << " " << ADC_code[269] << " " << ADC_code[270] << " " << ADC_code[271] << " " << ADC_code[272] << " " << ADC_code[273] << " " << ADC_code[274] << " " << ADC_code[275] << " " << ADC_code[276] << " " << ADC_code[277] << " " << ADC_code[278] << " " << ADC_code[279] << " " << ADC_code[280] << " " << ADC_code[281] << " " << ADC_code[282] << " " << ADC_code[283] << " " << ADC_code[284] << " " << ADC_code[285] << " " << ADC_code[286] << " " << ADC_code[287] << " " << ADC_code[288] << " " << ADC_code[289] << " " << ADC_code[290] << " " << ADC_code[291] << " " << ADC_code[292] << " " << ADC_code[293] << " " << ADC_code[294] << " " << ADC_code[295] << " " << ADC_code[296] << " " << ADC_code[297] << " " << ADC_code[298] << " " << ADC_code[299] << " " << ADC_code[300] << " " << ADC_code[301] << " " << ADC_code[302] << " " << ADC_code[303] << " " << ADC_code[304] << " " << ADC_code[305] << " " << ADC_code[306] << " " << ADC_code[307] << " " << ADC_code[308] << " " << ADC_code[309] << " " << ADC_code[310] << " " << ADC_code[311] << " " << ADC_code[312] << " " << ADC_code[313] << " " << ADC_code[314] << " " << ADC_code[315] << " " << ADC_code[316] << " " << ADC_code[317] << " " << ADC_code[318] << " " << ADC_code[319] << " " << ADC_code[320] << " " 
                //         << ADC_code[321] << " " << ADC_code[322] << " " << ADC_code[323] << " " << ADC_code[324] << " " << ADC_code[325] << " " << ADC_code[326] << " " << ADC_code[327] << " " << ADC_code[328] << " " << ADC_code[329] << " " << ADC_code[330] << " " << ADC_code[331] << " " << ADC_code[332] << " " << ADC_code[333] << " " << ADC_code[334] << " " << ADC_code[335] << " " << ADC_code[336] << " " << ADC_code[337] << " " << ADC_code[338] << " " << ADC_code[339] << " " << ADC_code[340] << " " << ADC_code[341] << " " << ADC_code[342] << " " << ADC_code[343] << " " << ADC_code[344] << " " << ADC_code[345] << " " << ADC_code[346] << " " << ADC_code[347] << " " << ADC_code[348] << " " << ADC_code[349] << " " << ADC_code[350] << " " << ADC_code[351] << " " << ADC_code[352] << " " << ADC_code[353] << " " << ADC_code[354] << " " << ADC_code[355] << " " << ADC_code[356] << " " << ADC_code[357] << " " << ADC_code[358] << " " << ADC_code[359] << " " << ADC_code[360] << " " << ADC_code[361] << " " << ADC_code[362] << " " << ADC_code[363] << " " << ADC_code[364] << " " << ADC_code[365] << " " << ADC_code[366] << " " << ADC_code[367] << " " << ADC_code[368] << " " << ADC_code[369] << " " << ADC_code[370] << " " << ADC_code[371] << " " << ADC_code[372] << " " << ADC_code[373] << " " << ADC_code[374] << " " << ADC_code[375] << " " << ADC_code[376] << " " << ADC_code[377] << " " << ADC_code[378] << " " << ADC_code[379] << " " << ADC_code[380] << " " << ADC_code[381] << " " << ADC_code[382] << " " << ADC_code[383] << " " << ADC_code[384] << " " 
                //         << ADC_code[385] << " " << ADC_code[386] << " " << ADC_code[387] << " " << ADC_code[388] << " " << ADC_code[389] << " " << ADC_code[390] << " " << ADC_code[391] << " " << ADC_code[392] << " " << ADC_code[393] << " " << ADC_code[394] << " " << ADC_code[395] << " " << ADC_code[396] << " " << ADC_code[397] << " " << ADC_code[398] << " " << ADC_code[399] << " " << ADC_code[400] << " " << ADC_code[401] << " " << ADC_code[402] << " " << ADC_code[403] << " " << ADC_code[404] << " " << ADC_code[405] << " " << ADC_code[406] << " " << ADC_code[407] << " " << ADC_code[408] << " " << ADC_code[409] << " " << ADC_code[410] << " " << ADC_code[411] << " " << ADC_code[412] << " " << ADC_code[413] << " " << ADC_code[414] << " " << ADC_code[415] << " " << ADC_code[416] << " " << ADC_code[417] << " " << ADC_code[418] << " " << ADC_code[419] << " " << ADC_code[420] << " " << ADC_code[421] << " " << ADC_code[422] << " " << ADC_code[423] << " " << ADC_code[424] << " " << ADC_code[425] << " " << ADC_code[426] << " " << ADC_code[427] << " " << ADC_code[428] << " " << ADC_code[429] << " " << ADC_code[430] << " " << ADC_code[431] << " " << ADC_code[432] << " " << ADC_code[433] << " " << ADC_code[434] << " " << ADC_code[435] << " " << ADC_code[436] << " " << ADC_code[437] << " " << ADC_code[438] << " " << ADC_code[439] << " " << ADC_code[440] << " " << ADC_code[441] << " " << ADC_code[442] << " " << ADC_code[443] << " " << ADC_code[444] << " " << ADC_code[445] << " " << ADC_code[446] << " " << ADC_code[447] << " " << ADC_code[448] << " " 
                //         << ADC_code[449] << " " << ADC_code[450] << " " << ADC_code[451] << " " << ADC_code[452] << " " << ADC_code[453] << " " << ADC_code[454] << " " << ADC_code[455] << " " << ADC_code[456] << " " << ADC_code[457] << " " << ADC_code[458] << " " << ADC_code[459] << " " << ADC_code[460] << " " << ADC_code[461] << " " << ADC_code[462] << " " << ADC_code[463] << " " << ADC_code[464] << " " << ADC_code[465] << " " << ADC_code[466] << " " << ADC_code[467] << " " << ADC_code[468] << " " << ADC_code[469] << " " << ADC_code[470] << " " << ADC_code[471] << " " << ADC_code[472] << " " << ADC_code[473] << " " << ADC_code[474] << " " << ADC_code[475] << " " << ADC_code[476] << " " << ADC_code[477] << " " << ADC_code[478] << " " << ADC_code[479] << " " << ADC_code[480] << " " << ADC_code[481] << " " << ADC_code[482] << " " << ADC_code[483] << " " << ADC_code[484] << " " << ADC_code[485] << " " << ADC_code[486] << " " << ADC_code[487] << " " << ADC_code[488] << " " << ADC_code[489] << " " << ADC_code[490] << " " << ADC_code[491] << " " << ADC_code[492] << " " << ADC_code[493] << " " << ADC_code[494] << " " << ADC_code[495] << " " << ADC_code[496] << " " << ADC_code[497] << " " << ADC_code[498] << " " << ADC_code[499] << " " << ADC_code[500] << " " << ADC_code[501] << " " << ADC_code[502] << " " << ADC_code[503] << " " << ADC_code[504] << " " << ADC_code[505] << " " << ADC_code[506] << " " << ADC_code[507] << " " << ADC_code[508] << " " << ADC_code[509] << " " << ADC_code[510] << " " << ADC_code[511] << " " 
                //         << ADC_code[512] << " " 
                //         << endl;                
            }

            // cout << event_ID << " " << timestamp << " " << source_ID << " "
            //         << Card_num << " " << Chip_num << " " << Chn_num << " " 
            //         << ADC_code[1] << " " << ADC_code[512] << " " 
            //         << endl;  

            if ((Packet[i + 1] & take_EOE_bit) != 0){
                source_ID = (Packet[i + 2] & take_SOURCE_ID_bits);
            }
            // cout << current_position << " ~ " << current_packet_length << endl;
        }        
    }

    // cout << ".dat file was written into the file : " << strcat(filename, ".txt") << ".\n";
    // strcpy(filename, file);
    cout << ".dat file was written into the file : " << strcat(filename, ".root") << ".\n";
    strcpy(filename, file);
    cout << "Total events numbers : " << total_event << endl;
    // xinhao.close();

	T->Write();
	f1->Close();

    delete[] Packet;
    delete[] file_origin_packet;

    return 1;
}

