t <html><head><title>AD Input</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("ad.cgx", 500);
t function plotADGraph() {
t  adVal1 = document.getElementById("ad_value1").value;
t  numVal1 = parseInt(adVal1, 16);
t  voltsVal1 = (3.3*numVal1)/4096;
t  tableSize1 = (numVal1*100/4096);
t  document.getElementById("ad_table1").style.width = (tableSize1 + '%');
t  document.getElementById("ad_volts1").value = (voltsVal1.toFixed(3) + ' V');
t  adVal2 = document.getElementById("ad_value2").value;
t  numVal2 = parseInt(adVal2, 16);
t  voltsVal2 = (3.3*numVal2)/4096;
t  tableSize2 = (numVal2*100/4096);
t  document.getElementById("ad_table2").style.width = (tableSize2 + '%');
t  document.getElementById("ad_volts2").value = (voltsVal2.toFixed(3) + ' V');
t }
t function periodicUpdateAd() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>AD Converter Input</h2>
t <p><font size="3">Esta p�gina permite al usuario ver por pantalla la variaci�n
t de los dos potenci�metros de la <b>App Board</b> en tiempo real. Al clickar en
t la opci�n <b>Periodic</b> podr� observarse la variaci�n mediante una barra. <br><br>
t La configuraci�n de pines es PC3 (POT1) y PC0 (POT2) mediante ADC.</font></p>
t <form action="ad.cgi" method="post" name="ad">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #ded410">
t  <th width=15%>Item</th>
t  <th width=15%>Value</th>
t  <th width=15%>Volts</th>
t  <th width=55%>Bargraph</th></tr>
t <tr><td><img src="pabb.gif">POT1:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c g 1  size="10" id="ad_value1" value="0x%03X"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c g 2  size="10" id="ad_volts1" value="%5.3f V"></td>
t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
c g 3 <td><table id="ad_table1" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
t <tr><td bgcolor="#0000FF">&nbsp;</td></tr></table></td></tr></table></td></tr>
t <tr><td><img src="pabb.gif">POT2:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c g 4  size="10" id="ad_value2" value="0x%03X"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c g 5  size="10" id="ad_volts2" value="%5.3f V"></td>
t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
c g 6 <td><table id="ad_table2" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
t <tr><td bgcolor="#2D873D">&nbsp;</td></tr></table></td></tr></table></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period
