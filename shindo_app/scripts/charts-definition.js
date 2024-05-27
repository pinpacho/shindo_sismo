// Create the charts when the web page loads
window.addEventListener('load', onload);

function onload(event){

  chartP = createIfFmaxChart();
}


// Create IfFmax Chart
function createIfFmaxChart() {
  var chart = new Highcharts.Chart({
    chart: { 
      renderTo: 'chart-IfFmax',
      type: 'spline'
    },
    series: [{
      name: 'MPU6050'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      spline: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#A62639' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { 
        second: '%H:%M:%S',
        minute: '%H:%M',
        hour: '%H:%M'
      },
      title: {
        text: 'Hora'
      }
    },
    yAxis: {
      title: { 
        text: 'IfFmax' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
