'use strict';

/*
 * Déclaration du module
 */
var devicesModule = angular.module('DomonetApp.devicesModule',[]);

/*
 * Contrôleur devicesList
 */
devicesModule.controller('devicesCtrl', ['$scope', '$http',
    function ($scope, $http) 
    {
      	$scope.gridsterOpts = {
          colWidth: 160,
          rowHeight: 160,
          margins: [20, 20],
      		outerMargin: false,
      		pushing: true,
      		floating: true,
      		draggable: {
      			enabled: true
      		},
      		resizable: {
      			enabled: false,
      			handles: ['n', 'e', 's', 'w', 'se', 'sw']
      		},
          mobileBreakPoint: 480, // if the screen is not wider that this, remove the grid layout and stack the items
          mobileModeEnabled: true
      	};
        
	      var url = window.location.protocol + "//" + window.location.host + "/web/pages/devices/devices.config.js";
        $http({
            method: "GET",
            url: url,
            headers: { 'Content-Type': 'application/json' }
        }).success(function (data) {
            $scope.template = data.template;
            $scope.extension = data.extension;
        }).error(function (data) {
            console.log(data);
        });

        var url = window.location.protocol + "//" + window.location.host + "/api/v1/devices";
        $http({
            method: "GET",
            url: url,
            headers: { 'Content-Type': 'application/json' }
        }).success(function (data) {
            var nbcol = 6;
            for(var i=0; i<data.devices.length; i++)
            {
              data.devices[i].col = (i%nbcol);
              data.devices[i].row = Math.floor((i+nbcol)/nbcol);
              data.devices[i].sizeX = 1;
              data.devices[i].sizeY = 1;
              data.devices[i].newValue = data.devices[i].value;
            }
            $scope.devices = data.devices;
        }).error(function (data) {
            alert("Impossible d'obtenir la liste des devices");
            console.log(data);
        });

        $scope.sendValue = function(device)
        {
          var msg = new Object();
          msg.structure = "xpl-cmnd";
          msg.target = device.module+"."+device.instance; 
          msg.class = "control.basic";
          msg.body = new Object();
          msg.body.device = device.name;
          msg.body.type = device.type;
          msg.body.current = device.newValue;
          
          var url = window.location.protocol + "//" + window.location.host + "/api/v1/messages/";
          $http.put(url, msg).
            success(function(data, status, headers, config) {
            device.value = device.newValue; 
            }).
            error(function(data, status, headers, config) {
              alert(data);
            });
        }
    }
]);
