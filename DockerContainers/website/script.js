var api_url = "http://192.168.2.34:8085/api/get_all_data";
var request = new Request(api_url);

jQuery(document).ready(function ($) {

    startLiveUpdate();

});

function startLiveUpdate () {
    setInterval(function () {
        fetch(api_url).then(function (response) {
            return response.json();
        }).then(function (data) {
            // console.log(data);
            updateTable("data_table", ['entry_id', 'dateheure', 'location', 'temperature', 'humidity'], data);
        }).catch(function (error) {
            console.log(error);
        });
    }, 1000);

}


function updateTable(tableId, fields, data) {
    $('#' + tableId).empty(); //not really necessary
    var rows = '<thead> <tr style="font-size: 20px; font-weight: bold; border-bottom: 2px solid black; margin: 20px; "> <th>compteur</th> <th>date/heure</th> <th>location</th> <th>température</th> <th>humidité</th> </tr> </thead>';
    $.each(data, function(index, item) {
        var row = '<tr>';
        $.each(fields, function(index, field) {
            if (field == "dateheure") {
                // var dateObject = new Date(Date.parse(item[field+'']));
                // var datetimeString = dateObject.toString();
                var datetimeString = item[field+''].replace("T", " ").replace(".000Z", "");
                row += '<td style="text-align: center;">' + datetimeString + '</td>';
            } else {
                row += '<td style="text-align: center;">' + item[field+''] + '</td>';
            }
            // row += '<td style="text-align: center;">' + item[field+''] + '</td>';
            // console.log(row);
        });
        rows += row + '<tr>';
    });
    
    $('#' + tableId).html(rows);
}