t <html><head><title>LED Control</title>
t <script language=JavaScript>
t function AllSW(st) {
t  for(i=0;i<document.form1.length;i++) {
t   if(document.form1.elements[i].type=="checkbox"){
t    document.form1.elements[i].checked=st;
t   }
t  }
t  document.form1.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Control LEDs: F429ZI & MBED AppBoard</h2>
t <p><font size="3">Mediante esta página web es posible encender/apagar los leds
t  de la tarjeta núcleo <b>F429ZI</b> como de la <b>MBED App Board</b>. Tiene dos
t  modos de funcionamiento: <b>Running Lights</b> y <b>Browser</b>. En el primero
t  los leds realizan una secuencia de ON/OFF dada en la función <b>BlinkLed</b>.
t  El segundo modo permite encender/apagar los leds seleccionados en las checkboxes.<br><br>
t  Los pines de conexión de la <b>F429ZI</b> son: LD1 (PB0), LD2 (PB7) y LD3 (PB11)
t  Los pines de conexión de la <b>App Board</b> son: R (PD11), G (PD12) y B (PD13).</font></p>
t <form action=leds.cgi method=post name=form1>
t <input type=hidden value="led" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#9aed93>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
t <td><img src=pabb.gif>LED control:</td>
t <td><select name="ctrl" onchange="submit();">
c b c <option %s>Browser</option><option %s>Running Lights</option></select></td></tr>
t <tr><td><img src=pabb.gif>LED MBED/F429ZI:</td>
t <td><table><tr valign="middle">
# Here begin the 'checkbox' definitions
c b 5 <td><input type=checkbox name=led5 OnClick="submit();" %s>R</td>
c b 4 <td><input type=checkbox name=led4 OnClick="submit();" %s>G</td>
c b 3 <td><input type=checkbox name=led3 OnClick="submit();" %s>B</td>
t <td width="5%"></td>
c b 2 <td><input type=checkbox name=led2 OnClick="submit();" %s>LD3</td>
c b 1 <td><input type=checkbox name=led1 OnClick="submit();" %s>LD2</td>
c b 0 <td><input type=checkbox name=led0 OnClick="submit();" %s>LD1</td>
t </font></table></td></tr>
t <tr><td><img src=pabb.gif>All LED diodes On or OFF</td>
t <td><input type=button value="&nbsp;&nbsp;ON&nbsp;&nbsp;&nbsp;" onclick="AllSW(true)">
t <input type=button value="&nbsp;&nbsp;OFF&nbsp;&nbsp;" onclick="AllSW(false)"></td></tr>
t </table></form>
i pg_footer.inc
. End of script must be closed with period.


