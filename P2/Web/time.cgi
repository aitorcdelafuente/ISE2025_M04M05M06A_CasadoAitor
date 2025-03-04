t <html><head><title>RTC</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout 
t var formUpdate = new periodicObj("time.cgx", 500);
t function plotTimeGraph() {
t  timeVal = document.getElementById("time").value;
t  dateVal = document.getElementById("date").value;
t  document.getElementById("time").value = (timeVal);
t  document.getElementById("date").value = (dateVal);
t }
t function periodicUpdateTime() {
t  if(document.getElementById("TiChkBox").checked == true) {
t   updateMultiple(formUpdate,plotTimeGraph);
t   Ti_elTime = setTimeout(periodicUpdateTime, formUpdate.period);
t  }
t  else
t   clearTimeout(Ti_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>Reloj en Tiempo Real</h2>
t <p><font size="2">This page shows you to configure the hour 
t and date, which is also displayed on LCD Module.</font></p>
t <form action="time.cgi" method="post" name="rtc">
t <input type="hidden" value="rtc" name="rtc">
t <table border=0 width=99%><font size="2">
t <tr style="background-color: #f8701d">
t  <th width=15%>Hora</th>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 1  size="10" id="hour" value="%s"></td></tr>
t  <th width=15%>Fecha</th>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 2  size="10" id="date" value="%s"></td>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotTimeGraph)">
t Periodic:<input type="checkbox" id="TiChkBox" onclick="periodicUpdateTime()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
