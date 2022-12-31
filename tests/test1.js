const io   = require('../../../../workspace/packages/io/dist/@youwol/io')
const geom = require('../../../../workspace/packages/geometry/dist/@youwol/geometry')
const fs   = require('fs')

// Generate
const dataframe = geom.generateEllipse({a:1, b:1, nbRings:25, density:8, center:[0, 0, 0]})

// Save init
fs.writeFileSync('/Users/fmaerten/data/mesh/surface.ts', io.encodeGocadTS(dataframe), 'utf8', err => {})

// Relax
dataframe.series.positions = geom.relaxMesh(dataframe.series.positions, dataframe.series.indices, 1000, 0.1)

// Translate for comparison
dataframe.series.positions = dataframe.series.positions.map( v => [v[0]+1.1, v[1], v[2]])

// Save relaxed
fs.writeFileSync('/Users/fmaerten/data/mesh/surface-relax.ts', io.encodeGocadTS(dataframe), 'utf8', err => {})
