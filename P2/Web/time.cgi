t <html><head><title>RTC</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("time.cgx", 500);
t function plotADGraph() {
t  timeVal = document.getElementById("time").value;
t  dateVal = document.getElementById("date").value;
t  document.getElementById("time").value = (timeVal);
t  document.getElementById("date").value = (dateVal);
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
t <h2 align="center"><br>Reloj en Tiempo Real</h2>
t <p><font size="2">Reloj que proporciona la fecha y la hora</font></p>
t <form action="time.cgi" method="post" name="rtc">
t <input type="hidden" value="ad" name="rtc">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t  <th width=50%> Valor a representar </th>
t  <th width=50%> Dato a representar </th></tr>
t <tr><td><img src="pabb.gif"> Hora del sistema </td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c h 1  size="10" id="time" value = "%s"></td>
t </tr>
t <tr><td><img src="pabb.gif"> Fecha del sistema </td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c h 2  size="10" id="date" value = "%s"></td>
t </tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period
