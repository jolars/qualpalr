/* el is the div, holding the rgl object as el.rglinstance,
     which holds x as el.rglinstance.scene
   x is the JSON encoded rglwidget.
*/


HTMLWidgets.widget({

  name: 'rglWebGL',

  type: 'output',

  initialize: function(el, width, height) {
    el.width = width;
    el.height = height;
    return {};

  },

  renderValue: function(el, x) {
    var rgl = new rglwidgetClass(), i, pel, player;
    
    rgl.initialize(el, x);
    rgl.initGL();
    
    /* We might have been called after (some of) the players were rendered.
       We need to make sure we respond to their initial values. */

    if (typeof x.players !== "undefined") {
      var players = [].concat(x.players);
      for (i = 0; i < players.length; i++) {
        pel = window[players[i]];
        if (typeof pel !== "undefined") {
          player = pel.rglPlayer;
          if (typeof player !== "undefined" && !player.initialized) {
            rgl.Player(pel, player);
            player.initialized = true;
          }
        } else
          rgl.alertOnce("Controller '" + players[i] + "' not found.");
      }
    }
    rgl.drag = 0;
    rgl.drawScene();
  },

  resize: function(el, width, height) {
    el.width = width;
    el.height = height;
    el.rglinstance.resize(el);
    el.rglinstance.drawScene();
  }

});
