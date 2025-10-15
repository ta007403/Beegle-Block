Beegle Block - Bitcoin Block Clock

#

Block Clock mini ฝีมือคนไทย น่ารักน่าชัง<br>
เหมาะแก่การตั้งโต๊ะทำงาน โต๊ะโชว์ของ ให้ชาวโลกได้รับรู้ว่าเราคือ bitcoiner ตัวจริงเสียงจริง<br>
เป็นการแสดงให้มวลมนุษยชาติได้ตระหนักถึงพลังแห่งความคิดสร้างสรรค์ของ Bitcoiner อย่างแท้ทรู<br>

Beegle Block เป็นนาฬิกา Block Clock ที่แสดงเลข Block ล่าสุดของ bitcoin block chain<br>
และเท่านั้นยังไม่พอ Beegle Block มี feature ดังต่อไปนี้<br>

– แสดงเลข ฺBlock ล่าสุดของ BTC block chain<br>
– แสดง ราคาของ bitcoin เป็น USD<br>
– แสดง ราคาของ bitcoin เป็น THB<br>
– แสดงอัตราแลกเปลี่ยนระหว่าง USD กับ THB<br>
– แสดงวันที่และเวลา<br>
– หน้าจอ Touch Screen 4.3 นิ้ว<br>
– ปุ่มปิดเปิด illuminate switch มี LED สวยงาม<br>
– วัสดุทำจากอลูมิเนียม รมดำ ให้ความรู้สึกเรียบหรู สุขุม นุ่มลึก<br>

ใช้งานง่าย เพียงแค่เสียบ Adapter +5V เข้ากับกล่อง Block Block จากนั้นทำการเปิด Switch<br>
แล้วเราจะสามารถใส่ SSID และ Password wifi ได้ทันที เท่านั้นยังไม่พอ ยังมี function การจดจำ SSID และ wifi password อีกด้วย<br>
นอกจากนั้นยังมีเสาอากาศสำหรับรับสัญญาณ wifi ได้ระยะไกลมากขึ้น<br>

หากคุณคิดว่า Block Clock mini ของ coinkite ราคาแพงเกินไป ลองทำเองก็ได้นะ<br>
Bitcoiner ไม่ควรพลาดด้วยประการทั้งปวง<br>

#

สิ่งที่มันทำก็คือ <br>
* Get date and time from api.timezonedb.com<br>
* Get BTC rate from coinmarketcap.com<br>
* Get block height from api.blockcypher.com<br>
* Get exchange rate from https://v6.exchangerate-api.com<br>

หากคุณอยากใช้ API นี้คุณก็สามารถไปสมัครได้ด้วยตัวคุณเอง

#

Display<br>
- ให้ทำการ Flash HMI firmware ของหน้าจอด้วยไฟล์ที่มีชื่อว่า Beegle Box Display V2.2<br>
- หน้าจอเป็นยี่ห้อ Nextion รุ่น NX4827T043<br>

#

Pre-requisite<br>
- Nextion display library is needed<br>
- ArduinoJson 5.x is needed<br>
- Copy both library to C:\Users\xxx\Documents\Arduino\libraries<br>
- Just unzip and leave it there<br>
- ESP8266 tool chain library (Click at file --> Preferences)<br>
- http://arduino.esp8266.com/stable/package_esp8266com_index.json<br>
- Put the upper command to blank --> Additional Board Manager<br>
- Now Click on "Tools" --> click on "Board" a drop-down menu will be popped. Click on "Boards Manager"<br>
- Then search for ESP8266 and install it (choose version 3.0.0 only).<br>
- After that we have to choose ESP8266 Generic Board.<br>

#

วิธีการ Modify เสา Wifi

อันดับแรกเลยให้คุณเอา Cutter กรีด Copper Line ที่มันเป็นเสาเดิมออกก่อน (ขั้นตอนนี้สำคัญ)
![F7VSIKAJZD6U9LC](https://github.com/user-attachments/assets/8b3f16be-ce7e-4616-89d3-ecb8e7476a54)

ขั้นที่ 2 ให้คุณปลอกปลายสายแล้วบัดกรีสาย core กับสาย shield ลงไปบนบอร์ด
![F7SLLJ2JZD6U9LV](https://github.com/user-attachments/assets/9ba04c51-59e1-4f1e-bfa0-036b9aa65161)

ขั้นที่ 3 จากนั้นให้เอากาวร้อนมาติด
![FE1J00WJZD6U9LX](https://github.com/user-attachments/assets/76b5ec25-8c12-44f4-aed6-8c086b5b9543)

ขั้นที่ 4 ก็จะได้ลักษณะแบบนี้
![FNRPY0QJZD6UIS9](https://github.com/user-attachments/assets/3fc1edf3-4d17-4e94-be25-5e563f0a2d70)

#

วิธีการติดตั้ง

ขั้นที่ 1 ใส่ DC jack ใส่ switch ใส่เสา wifi ให้เรียบร้อย นอกนั้นดูรูปเอาละกัน (ESP8266 คุยกับ Nextion ด้วย UART 1 นะ)
![20220617_155815](https://github.com/user-attachments/assets/05b19cec-20c5-46f8-9943-cbe621ccced9)

![20220617_155825](https://github.com/user-attachments/assets/a68959ec-ddef-40e1-9000-79c099cb3081)

![20220617_161359](https://github.com/user-attachments/assets/b8c0349d-1f27-40b8-8768-4e1c70574993)

![20220617_161408](https://github.com/user-attachments/assets/a8b6b86d-92a3-430c-bf88-69ef60f21dbd)

