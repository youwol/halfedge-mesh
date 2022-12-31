# Get the border of 3D triangulated surfaces + other algos

This port is based on Halfedge_Mesh.h written By Keenan Crane for 15-462 Assignment 2.
and updated by Max Slater for Fall 2020.

See https://github.com/CMU-Graphics/Scotty3D for more information.

## API
```ts
/**
 * Get the borders of the mesh. For the moment. Return
 * paire of vertices (x,y,z) to define an edge
 */
const borderPositions = borders(positions: number[], indices: number[]): number[]

const nodeIds = borderIds(positions: number[], indices: number[]): number[]

const newPositions = relax(positions: number[], indices: number[], nbIter: number): number[]
```

## Usage
```js
import { borders } from 'mesh'

// -------------------------------
// Generate the border(s): The API
// -------------------------------
const b = borders(vertices, triangles)

// ---------------------------------
// Draw the border(s) using three.js
// ---------------------------------
const indices = new Array(borders.length/6).fill(0).map( (v,i) => i)

const geometry = new THREE.BufferGeometry()
geometry.setAttribute('position', new THREE.BufferAttribute(new Float32Array(borders), 3))
geometry.setIndex(indices)

const material = new THREE.LineBasicMaterial({
    linewidth: 1,
    color    : new THREE.Color(color?color:"#000000")
})

scene.add( new THREE.LineSegments(geometry, material) )
```
