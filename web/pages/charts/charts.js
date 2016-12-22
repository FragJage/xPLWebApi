'use strict';
/*
 * Déclaration du module
 */
var chartsModule = angular.module('DomonetApp.chartsModule',[]);

/*
 * Contrôleur
 */
chartsModule.controller('chartsCtrl', ['$scope', '$rootScope', '$http', function ($scope, $rootScope, $http) {

    $scope.periode = "J";
    $scope.mode = "G";
    $scope.debut = moment().subtract(1,'day');
    $scope.fin = moment();
    
    var url = window.location.protocol + "//" + window.location.host + "/api/v1/datalogger/devices/?start=2015-01-01%2000:00:00&end=2016-12-31%2000:00:00";
    $http({
        method: "GET",
        url: url,
        headers: { 'Content-Type': 'application/json' }
    }).success(function (data) {
        $scope.devices = data.devices;
        if(data.devices.length>0) addSerie(data.devices[0]);
    }).error(function (data) {
        console.log(data);
    });
    
    function addSerie(device)
    {
      var url = window.location.protocol + "//" + window.location.host + "/api/v1/datalogger/values/"+device+"/?start="+$scope.debut.format('YYYY-MM-DD HH:mm:ss')+"&end="+$scope.fin.format('YYYY-MM-DD HH:mm:ss')+"&number=96";
      $http({
          method: "GET",
          url: url,
          headers: { 'Content-Type': 'application/json' }
      }).success(function (data) {
          var myValues = new Array();
          for(var i=0; i<data.values.length; i++)
          {
            var myValue = new Array();
            myValue.push(Date.parse(data.values[i].time));
            myValue.push(parseFloat(data.values[i].value));
            myValues.push(myValue);
          }
          for(var i=0; i<$scope.chartConfig.series.length; i++)
          {
            if($scope.chartConfig.series[i].name==device)
            {
               $scope.chartConfig.series[i].data = myValues;
               console.log("Find "+device)
               break;
            } 
          }
          if(i==$scope.chartConfig.series.length)
          {
            $scope.chartConfig.series.push({
                data: myValues,
                name: device
            })
            console.log("Add "+device)
          }
      }).error(function (data) {
          console.log(data);
      });
    }

    function refresh()
    {
      if($scope.mode=='F')
      {
        $scope.fin.endOf('day');
        if($scope.periode=='S') $scope.fin.endOf('week');;
        if($scope.periode=='M') $scope.fin.endOf('month');;
      }
      else
      {
        $scope.fin.set({'hour': moment().hour(), 'minute': moment().minute(), 'seconde': 0});
        if($scope.periode=='S') $scope.fin.weekday(moment().weekday());
        if($scope.periode=='M') $scope.fin.date(moment().date());
      }

      switch($scope.periode)
      {
        case "J":
          $scope.debut = moment($scope.fin).subtract(1,'day');
          break;
        case "S":
          $scope.debut = moment($scope.fin).subtract(7,'day');
          break;
        case "M":
          $scope.debut = moment($scope.fin).subtract(1,'month');
          break;
      }
      
      var devices = new Array();
      for(var i=0; i<$scope.chartConfig.series.length; i++)
        devices.push($scope.chartConfig.series[i].name);

      for(var i=0; i<devices.length; i++)
        addSerie(devices[i]);
    }
    
    $scope.setPeriode = function (periode)
    {
      $scope.periode = periode;
      refresh();
    } 

    $scope.setMode = function (mode)
    {
      $scope.mode = mode;
      refresh();
    } 

    $scope.setDecale = function (sens)
    {
console.log($scope.fin.format("YYYY-MM-DD HH:mm:ss"));
      switch($scope.periode)
      {
        case "J":
          $scope.fin.add(sens,'day');
          break;
        case "S":
          $scope.fin.add(7*sens,'day');
          break;
        case "M":
          $scope.fin.add(sens,'month');
          break;
      }
console.log($scope.fin.format("YYYY-MM-DD HH:mm:ss"));
      refresh();
    } 
    
    $scope.addDevice = function (device) 
    {
      for(var i=0; i<$scope.chartConfig.series.length; i++)
      {
        if($scope.chartConfig.series[i].name==device)
        {
          $scope.chartConfig.series.splice(i, 1);
          return;        
        } 
      }
      addSerie(device);
    };

    $scope.selDevice = function (device) 
    {
      for(var i=0; i<$scope.chartConfig.series.length; i++)
      {
        if($scope.chartConfig.series[i].name==device) return true;        
      }
      return false;
    };

    $rootScope.$broadcast('waiting', false);

    $scope.chartConfig = {
        options: {
            chart: {
                type: 'line'
            }
        },
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: { // don't display the dummy year
                month: '%e. %b',
                year: '%b'
            },
            title: {
                text: 'Date'
            }
        },        
        tooltipzzz: {
            headerFormat: '<b>{series.name}</b><br>',
            pointFormat: '{point.x:%e. %b}: {point.y:.2f} m'
        },        
        plotOptions: {
            spline: {
                marker: {
                    enabled: true
                }
            }
        },
        series: [
        ],
        title: {
            text: ''
        }
    }
}]);