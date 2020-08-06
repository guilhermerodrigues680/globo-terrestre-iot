// Cesium.Ion.defaultAccessToken = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJlNTg4MmJjZC0wZDZkLTQxODMtOGRhZi0wZWU2ZDI1ODY3ZGEiLCJpZCI6MzE4NTAsInNjb3BlcyI6WyJhc3IiLCJnYyJdLCJpYXQiOjE1OTYwNjMyMzV9.CnBln1kTTLjYJkCS1elia1_SoI6280GumVIJvq_q0VQ';
function initViewerCesium(containerId) {
  const viewer = new Cesium.Viewer('cesiumContainer', {
    imageryProvider: new Cesium.OpenStreetMapImageryProvider({
      url: 'https://a.tile.openstreetmap.org/'
    }),
    FullscreenButton: false,
    navigationHelpButton: false,
    animation: false,
    timeline: false,
  });
  // viewer.fullscreenButton.container.style.visibility = 'hidden';
  return viewer;
}

function initViewerCesium1(containerId) {
  //Initialize the viewer widget with several custom options and mixins.
  const viewer = new Cesium.Viewer('cesiumContainer', {
    //Start in Columbus Viewer
    sceneMode: Cesium.SceneMode.COLUMBUS_VIEW,
    //Use Cesium World Terrain
    terrainProvider: Cesium.createWorldTerrain(),
    //Hide the base layer picker
    baseLayerPicker: false,
    //Use OpenStreetMaps
    imageryProvider: new Cesium.OpenStreetMapImageryProvider({
      url: 'https://a.tile.openstreetmap.org/'
    }),
    skyBox: new Cesium.SkyBox({
      sources: {
        positiveX: 'stars/TychoSkymapII.t3_08192x04096_80_px.jpg',
        negativeX: 'stars/TychoSkymapII.t3_08192x04096_80_mx.jpg',
        positiveY: 'stars/TychoSkymapII.t3_08192x04096_80_py.jpg',
        negativeY: 'stars/TychoSkymapII.t3_08192x04096_80_my.jpg',
        positiveZ: 'stars/TychoSkymapII.t3_08192x04096_80_pz.jpg',
        negativeZ: 'stars/TychoSkymapII.t3_08192x04096_80_mz.jpg'
      }
    }),
    // Show Columbus View map with Web Mercator projection
    mapProjection: new Cesium.WebMercatorProjection()
  });

  //Add basic drag and drop functionality
  viewer.extend(Cesium.viewerDragDropMixin);

  //Show a pop-up alert if we encounter an error when processing a dropped file
  viewer.dropError.addEventListener(function (dropHandler, name, error) {
    console.log(error);
    window.alert(error);
  });

  return viewer
}

function initViewerCesium0(containerId) {
  const viewer = new Cesium.Viewer('cesiumContainer');
  viewer.animation.container.style.visibility = 'hidden';
  viewer.timeline.container.style.visibility = 'hidden';
  viewer.fullscreenButton.container.style.visibility = 'hidden';
  return viewer;

  // var viewer = new Cesium.Viewer('cesiumContainer', {
  //   terrainProvider: Cesium.createWorldTerrain()
  // });
  // var tileset = viewer.scene.primitives.add(
  //   new Cesium.Cesium3DTileset({
  //     url: Cesium.IonResource.fromAssetId(134342),
  //   })
  // );
}

const viewer = initViewerCesium('cesiumContainer');
// viewer.zoomTo(tileset);

const bhCoordArr = [-19.88014, -43.90813]; // [lat, lon]
const bhCoord = {
  lat: bhCoordArr[0],
  lon: bhCoordArr[1]
};

const point = {
  name: 'Ana Lúcia',
  // position : Cesium.Cartesian3.fromDegrees(-75.166493, 39.9060534),
  position: Cesium.Cartesian3.fromDegrees(bhCoord.lon, bhCoord.lat),
  point: {
    pixelSize: 5,
    color: Cesium.Color.RED,
    outlineColor: Cesium.Color.WHITE,
    outlineWidth: 2
  },
  label: {
    text: 'Citizens Bank Park',
    font: '14pt monospace',
    style: Cesium.LabelStyle.FILL_AND_OUTLINE,
    outlineWidth: 2,
    verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
    pixelOffset: new Cesium.Cartesian2(0, -9)
  }
};

const citizensBankPark = viewer.entities.add(point);
// viewer.zoomTo(viewer.entities);

setTimeout(()=>{
  viewer.scene.view.camera.flyTo({
    destination: Cesium.Cartesian3.fromDegrees(bhCoord.lon, bhCoord.lat, 200.0),
    orientation: {
      heading: Cesium.Math.toRadians(0.0),
      pitch: Cesium.Math.toRadians(-90.0),
      roll: 0.0
    }
  });
}, 5000);

////
var camera = viewer.scene.camera;
// var currentCartesian = camera.position.clone();
// var currentCartographic = Cesium.Ellipsoid.WGS84.cartesianToCartographic(currentCartesian); // then you can use:
// Cesium.Math.toDegrees(currentCartographic.longitude)

camera.moveStart.addEventListener((e) => {
  console.log('moveStart', e);
})
camera.moveEnd.addEventListener((e) => {
  console.log('moveEnd', e);
})

// viewer.scene.screenSpaceCameraController.inertiaSpin = 0;
// viewer.scene.screenSpaceCameraController.inertiaTranslate = 0;
// viewer.scene.screenSpaceCameraController.inertiaZoom = 0;
