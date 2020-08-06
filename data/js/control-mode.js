import * as cesiumHelper from "./cesium-helper.js";
import * as wsHelper from './control-mode-ws.js'

function debugToHTML(message) {
  const divDebug = document.querySelector('#debug');
  divDebug.innerHTML = message;
}

function timerGlobeCoord(camera) {
  const coordCamera = {
    lastCoord: cesiumHelper.getCameraDegreesCoord(camera),
    currentCoord: null
  };
  setInterval(() => {
    const coord = cesiumHelper.getCameraDegreesCoord(camera);
    coordCamera.currentCoord = coord;

    if (coordCamera.currentCoord.lat !== coordCamera.lastCoord.lat
      && coordCamera.currentCoord.lon !== coordCamera.lastCoord.lon
      && coordCamera.currentCoord.height !== coordCamera.lastCoord.height
    ) {
      console.log(coord);
      debugToHTML(JSON.stringify(coord));
      coordCamera.lastCoord = coord;
      // wsHelper.sendCoord(coord);
      const coordESP = {
        lat: Math.round((coord.lat + Number.EPSILON) * 1e3) / 1e3,
        lon: Math.round((coord.lon + Number.EPSILON) * 1e3) / 1e3,
        height: Math.round(coord.height),
      }
      wsHelper.sendCoord2(coordESP);
    }
  }, 500);
}

function initSearch(viewer) {
  const btnSearchDOM = document.querySelector('#js-btn-search');

  btnSearchDOM.addEventListener('click', (e) => {
    const searchDOM = document.querySelector('#js-text-search');
    const text = searchDOM.value;
    const textURI = encodeURI(text);
    const url = `https://nominatim.openstreetmap.org/search?q=${textURI}&format=json&polygon_geojson=1`;
    console.debug(url);

    fetch(url)
      .then(response => response.json())
      .then(data => {
        viewer.entities.removeAll();
        const typePolygon = data[0].geojson.type;
        console.debug(data[0].geojson.type, data[0].geojson)

        if (typePolygon === 'MultiPolygon') {
          const coordinates = data[0].geojson.coordinates;
          const entitiesPol = [];

          for (const coord of coordinates) {
            const limits = coord.flat(Infinity);
            console.debug(coord, limits);

            const wyoming = viewer.entities.add({
              polygon: {
                hierarchy: Cesium.Cartesian3.fromDegreesArray(limits),
                height: 0,
                material: Cesium.Color.GOLD.withAlpha(0.5),
                outline: true,
                outlineColor: Cesium.Color.BLACK
              }
            });

            entitiesPol.push(wyoming);
          }

          viewer.flyTo(entitiesPol, {
            duration: 5.0,
            maximumHeight: 0.0,
            offset: new Cesium.HeadingPitchRange(0.0, -90.0, 12e6)
          })
            .then(() => {
              const label = {
                name: data[0].display_name,
                position: Cesium.Cartesian3.fromDegrees(data[0].lon, data[0].lat),
                label: {
                  text: data[0].display_name.slice(0,21),
                  font: '14pt monospace',
                  style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                  outlineWidth: 2,
                  verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                  // pixelOffset: new Cesium.Cartesian2(0, -9)
                }
              };
              viewer.entities.add(label);

              console.log('FIM 1');
              viewer.flyTo(entitiesPol, {
                duration: 5.0,
                maximumHeight: 0.0,
                offset: new Cesium.HeadingPitchRange(0.0, -90.0, 5e6)
              })
                .then(() => console.log('FIM 2'));
            });


        } else {
          // 'Polygon' ?
          const limits = data[0].geojson.coordinates[0].flat(Infinity);

          const wyoming = viewer.entities.add({
            polygon: {
              // hierarchy: Cesium.Cartesian3.fromDegreesArray([
              //   -43.90857, -19.88017,
              //   -43.90655, -19.88169,
              //   -43.90697, -19.88543,
              //   -43.90857, -19.88815,
              // ]),
              hierarchy: Cesium.Cartesian3.fromDegreesArray(limits),
              height: 0,
              material: Cesium.Color.RED.withAlpha(0.5),
              outline: true,
              outlineColor: Cesium.Color.BLACK
            }
          });

          // viewer.zoomTo(wyoming, new Cesium.HeadingPitchRange(0.0, -90.0, 12e6))
          // .then(() => console.log('FIM'));
          viewer.flyTo(wyoming, {
            duration: 5.0,
            maximumHeight: 0.0,
            offset: new Cesium.HeadingPitchRange(0.0, -90.0, 12e6)
          })
            .then(() => {
              const label = {
                name: data[0].display_name,
                position: Cesium.Cartesian3.fromDegrees(data[0].lon, data[0].lat),
                label: {
                  text: data[0].display_name.slice(0,21),
                  font: '14pt monospace',
                  style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                  outlineWidth: 2,
                  verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                  // pixelOffset: new Cesium.Cartesian2(0, -9)
                }
              };
              viewer.entities.add(label);

              console.log('FIM 1');
              viewer.flyTo(wyoming, {
                duration: 5.0,
                maximumHeight: 0.0,
                offset: new Cesium.HeadingPitchRange(0.0, -90.0, 5e6)
              })
                .then(() => console.log('FIM 2'));
            });
          console.log(viewer, wyoming)
        }
      })
  });
}

function initViewerCesium(containerId) {

  const extent = Cesium.Rectangle.fromDegrees(-45, 50, -45, -90);
  Cesium.Camera.DEFAULT_VIEW_RECTANGLE = extent;
  Cesium.Camera.DEFAULT_VIEW_FACTOR = 0;

  const openStreetMap = new Cesium.OpenStreetMapImageryProvider({
    url: 'https://a.tile.openstreetmap.org/'
  });

  const positron = new Cesium.UrlTemplateImageryProvider({
    url : 'http://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}.png',
    credit : 'Map tiles by CartoDB, under CC BY 3.0. Data by OpenStreetMap, under ODbL.'
  });

  const tms = new Cesium.UrlTemplateImageryProvider({
    url : Cesium.buildModuleUrl('Assets/Textures/NaturalEarthII') + '/{z}/{x}/{reverseY}.jpg',
    credit : '© Analytical Graphics, Inc.',
    tilingScheme : new Cesium.GeographicTilingScheme(),
    maximumLevel : 5
  });

  const esri = new Cesium.ArcGisMapServerImageryProvider({
      url : 'https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer'
  });

  const esriNatGeo = new Cesium.ArcGisMapServerImageryProvider({
      url : 'https://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer'
  });

  const viewer = new Cesium.Viewer(containerId, {
    // imageryProvider: esriNatGeo,
    imageryProvider: tms,
    FullscreenButton: false,
    navigationHelpButton: false,
    animation: false,
    timeline: false,
  });
  // viewer.fullscreenButton.container.style.visibility = 'hidden';
  return viewer;
}

function initViewerCesium00(containerId) {
  const viewer = new Cesium.Viewer('cesiumContainer', {
    terrainProvider: new Cesium.ArcGISTiledElevationTerrainProvider({
      url: 'https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer'
    })
  });

  viewer.camera.setView({
    destination: Cesium.Cartesian3.fromDegrees(-121.82283810552254, 36.20978697451458, 2830.3061574691405),
    orientation: {
      heading: Cesium.Math.toRadians(57.53812991892656),
      pitch: Cesium.Math.toRadians(-19.74361760510513),
      roll: Cesium.Math.toRadians(0.164340324995592)
    }
  });
  return viewer;
}

cesiumHelper.configDefaultTokenCesium();
const viewer = initViewerCesium('cesiumContainer');
const camera = viewer.scene.camera;
timerGlobeCoord(camera);
const bhCoordArr = [-19.88014, -43.90813]; // [lat, lon]
// const anaLuciaPoint = cesiumHelper.addPointToMap(bhCoordArr, 'anaLucia', 'Ana Lúcia', viewer);
// console.log(anaLuciaPoint);
console.log(viewer);
initSearch(viewer);


const point = {
  name: name,
  // position : Cesium.Cartesian3.fromDegrees(-75.166493, 39.9060534),
  position: Cesium.Cartesian3.fromDegrees(0, 0),
  label: {
    text: 'text',
    font: '14pt monospace',
    style: Cesium.LabelStyle.FILL_AND_OUTLINE,
    outlineWidth: 2,
    verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
    pixelOffset: new Cesium.Cartesian2(0, -9)
  }
};
// viewer.entities.add(point);
console.log(point)

// wsHelper.initWebSocker('192.168.2.108', 8080);
// wsHelper.initWebSocker('192.168.2.108', 8080);
wsHelper.initWebSocker('192.168.2.108', 81);
