`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2018/05/08 12:28:11
// Design Name: 
// Module Name: display
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module display(clk_axi,reset,F,mmg,d1_wx);
      input clk_axi;
      input reset;
      input [31:0]F;
      output reg [7:0]mmg;
      output reg [7:0]d1_wx=8'b11111110;
      reg [3:0]d_in=4'b0000;
      wire clk_400Hz;
      reg [17:0]count_tmp=17'b0;
      assign clk_400Hz=count_tmp[16];
      always@(posedge clk_axi)begin
      if(!reset)
        count_tmp<=18'b0;
      else
            count_tmp<=count_tmp+18'b1;
      end
 always @(posedge clk_400Hz)  
      d1_wx <= {d1_wx[6:0],d1_wx[7]}; 
  always @(d1_wx,F) begin 
      case(d1_wx)
          8'b11111110:d_in = F[3:0];
          8'b11111101:d_in = F[7:4];
       8'b11111011:d_in= F[11:8];  
       8'b11110111:d_in= F[15:12];
          8'b11101111:d_in= F[19:16];
          8'b11011111:d_in= F[23:20];
          8'b10111111:d_in= F[27:24];
          8'b01111111:d_in= F[31:28];
      default:d_in=4'b1111; 
      endcase
      end
  //译码功能实现，完成0~f的显示
  always @(d_in,reset)begin  
  if(!reset)
      mmg= 8'b1100_0000;//0  
  else  
      case(d_in)  
          4'h0:mmg=8'b11000000;//0  
          4'h1:mmg=8'b11111001;//1  
          4'h2:mmg=8'b10100100;//2  
          4'h3:mmg=8'b10110000;//3  
          4'h4:mmg=8'b10011001;//4  
          4'h5:mmg=8'b10010010;//5  
          4'h6:mmg=8'b10000010;//6  
          4'h7:mmg=8'b11111000;//7  
          4'h8:mmg=8'b10000000;//8  
          4'h9:mmg=8'b10010000;//9  
          4'ha:mmg=8'b10001000;//a  
          4'hb:mmg=8'b10000011;//b  
          4'hc:mmg=8'b11000110;//c  
          4'hd:mmg=8'b10100001;//d  
          4'he:mmg=8'b10000111;//e  
          4'hf:mmg=8'b10001110;//f  
              //4'hf:mmg=8'b1111_1111;//不显示  
          default : mmg= 8'b1100_0000;//0  
      endcase  
 end          
     
   
endmodule
