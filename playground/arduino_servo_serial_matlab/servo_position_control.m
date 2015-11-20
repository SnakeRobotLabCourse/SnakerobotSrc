s = serial('COM3');  % set the serial port
set(s,'BaudRate',115200);  %speed
fopen(s);  %open the serial port

interval = 1000;  %
passo = 1;
t = 0;
x = 0;
while(t<interval)
    b = str2num(fgetl(s)); 
    x = [x,b];% use fgets(s) to read data from the buffer unitl the stop character, so we need to use function  Serial.println() in the Arduino program          

    %plot(t,x);
    %grid
    t = t+passo;
    drawnow;
end
L=length(x);
time=zeros(1,L);
for i=0:L-1
    time(1,(i+1))=0.0115*i;
end
plot(time(2:size(x,2)),x(2:size(x,2)));
grid on;
fclose(s);  %close the serial port
delete(s);                                                                 
clear s;                                                                  

index = find(x == 2002);
stable_t = time(index)