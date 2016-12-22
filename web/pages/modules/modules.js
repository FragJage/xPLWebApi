'use strict';

/*
 * Déclaration du module
 */
var modulesModule = angular.module('DomonetApp.modulesModule',[]);

/*
 * Contrôleur modulesCtrl
 */
modulesModule.controller('modulesCtrl', ['$scope', '$rootScope', '$http',
    function ($scope, $rootScope, $http) 
    {
        function displayModuleList(mustSetModule)
        {
          var url = window.location.protocol + "//" + window.location.host + "/api/v1/modules";
          $http({
              method: "GET",
              url: url,
              headers: { 'Content-Type': 'application/json' }
          }).success(function (data) {
              $scope.modules = data.modules;
              if((mustSetModule)&&($scope.modules.length>0)) $scope.setModule($scope.modules[0].name);
              $rootScope.$broadcast('waiting', false);
          }).error(function (data) {
              alert("Impossible d'obtenir la liste des modules");
              console.log(data);
          });
        }
        
      /*
        var url = window.location.protocol + "//" + window.location.host + "/web/modules/modules.config.js";
        $http({
            method: "GET",
            url: url,
            headers: { 'Content-Type': 'application/json' }
        }).success(function (data) {
            $scope.template = data.template;
        });
        */

        displayModuleList(true);
        
        $scope.$on('refreshModuleList', function (event)
        {
          displayModuleList(false); 
        });

        $scope.setModule = function (moduleName) 
        {
          $rootScope.$broadcast('waiting', true);
          $rootScope.$broadcast('setModule', moduleName);
        };
        /*
        // Déclencher le template des détails d'un module
        $scope.setModule = function (moduleName) 
        {
          $scope.selectedModule = moduleName;
          $scope.device = moduleName.split(".")[0];
        };

        // Envoyer le nom du module
        $scope.getModule = function (moduleName) 
        {
          $rootScope.$broadcast('waiting', true);
          $rootScope.$broadcast('setModule', moduleName);
        };
        */
    }
]);

/*
 * Contrôleur moduleCtrl
 */
modulesModule.controller('moduleCtrl', ['$scope', '$rootScope', '$http',
    function ($scope, $rootScope, $http) 
    {
        $scope.saveConfig = function()
        {
          $rootScope.$broadcast('waiting', true);
          var url = window.location.protocol + "//" + window.location.host + "/api/v1/modules/"+$scope.module.name;
          $http.put(url, $scope.module.config).
            success(function(data, status, headers, config) 
            {
              if($scope.module.name!=data) $rootScope.$broadcast('refreshModuleList');
              getModule(data, false);
              $rootScope.$broadcast('waiting', false);
            }).
            error(function(data, status, headers, config) 
            {
              $rootScope.$broadcast('waiting', false);
              alert("Argh, c'est raté !");
            });
        }

        $scope.optionAdd = function(key)
        {
          $scope.module.config[key].value.push("");
          $scope.configSplit[key].push(new Array());
        }

        $scope.optionRemove = function(key, value)
        {
          var index = $scope.module.config[key].value.indexOf(value);
          $scope.module.config[key].value.splice(index, 1);
          $scope.configSplit[key].splice(index, 1);   
        }

        $scope.refresh = function()
        {
          $rootScope.$broadcast('waiting', true);
          getModule($scope.module.name, true);
        }
        
        $scope.toAdvanceConfig = function()
        {
          location.assign("#/config/"+$scope.module.name);
        }

        function markModif(newValue, oldValue, scope)
        {
          scope.isModified = !isEqual(newValue, scope.configMemo);
        };
        
        function getModule(module, refresh)
        {
          var url = window.location.protocol + "//" + window.location.host + "/api/v1/modules/"+module;
          if(refresh) url=url+"?action=refresh";
          $http({
              method: "GET",
              url: url,
              headers: { 'Content-Type': 'application/json' }
          }).success(function (data) {
              $scope.module = data;
              $scope.configMemo = clone(data.config);
              $rootScope.$broadcast('waiting', false);
              $scope.$watch('module.config', markModif, true);
          }).error(function (data) {
              alert("Impossible d'obtenir les informations du module");
              console.log(data);
              $rootScope.$broadcast('waiting', false);
          });
        }
        
        $scope.$on('setModule', function (event, module)
        {
          getModule(module, false); 
        });
    }
]);

/*
 * Contrôleur configCtrl
 */
modulesModule.controller('configCtrl', ['$scope', '$routeParams', '$rootScope', '$http',
    function ($scope, $routeParams, $rootScope, $http) 
    {
      $scope.module = $routeParams.id; 

      $scope.saveConfig = function()
      {
        $rootScope.$broadcast('waiting', true);
        var max = $scope.advance.config.length
        for (var i=0; i<max; i++) 
        { 
          if ($scope.advance.config[i].device == "")
          {
            $scope.advance.config.splice(i, 1);
            i--;
            max--;
          } 
        }
        var url = window.location.protocol + "//" + window.location.host + "/api/v1/advanceconfig/"+$scope.module;
        $http.put(url, $scope.advance.config).
          success(function(data, status, headers, config) 
          {
            getConfig($scope.module);
            $rootScope.$broadcast('waiting', false);
          }).
          error(function(data, status, headers, config) 
          {
            $rootScope.$broadcast('waiting', false);
            alert("Argh, c'est raté !");
          });
      }

      $scope.optionAdd = function()
      {
        $scope.advance.config.push(clone($scope.emptyConfig));
      }

      $scope.optionRemove = function(value)
      {
        var index = $scope.advance.config.indexOf(value);
        $scope.advance.config.splice(index, 1);
      }

      function markModif(newValue, oldValue, scope)
      {
        scope.isModified = !isEqual(newValue, scope.configMemo);
      };
      
      function createEmptyConfig(configObj)
      {
        var temp = new configObj.constructor();
        for(var key in configObj) if (configObj.hasOwnProperty(key))
        {
          switch(configObj[key]["type"])
          {
            case "boolean" : 
              temp[key] = false;
              break;
            case "integer" :
              temp[key] = 0;
              break;
            case "float" :
              temp[key] = 0.0;
              break;
            default :
              temp[key] = "";
          }
        }
        return temp;
      };

      function getConfig(module)
      {
        $rootScope.$broadcast('waiting', true);
        var url = window.location.protocol + "//" + window.location.host + "/api/v1/advanceconfig/"+module;
        $http({
            method: "GET",
            url: url,
            headers: { 'Content-Type': 'application/json' }
        }).success(function (data) {
            $scope.emptyConfig = createEmptyConfig(data.format);
            $scope.advance = data;
            if($scope.advance.config == undefined)
            {
              $scope.advance.config = new Array();
              $scope.optionAdd();
            }
            $scope.configMemo = clone(data.config);
            $scope.$watch('advance.config', markModif, true);
            $rootScope.$broadcast('waiting', false);
        }).error(function (data) {
            alert("Impossible d'obtenir la configuration du module");
            console.log(data);
            $rootScope.$broadcast('waiting', false);
        });
      }
      getConfig($scope.module);
    }
]);
