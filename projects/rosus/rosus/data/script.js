document.addEventListener('DOMContentLoaded', () => {
    fetch('/data').then(response => response.json()).then(data => {
        document.querySelector('#ssid').innerHTML = data.wifi_ssid
        document.querySelector('#ip').innerHTML = data.wifi_local_ip
    })
})

const object3D = document.getElementById('3Dcube')

let width3Dobject = object3D.parentElement.clientWidth
let height3Dobject = object3D.parentElement.clientHeight

// Scene
const scene = new THREE.Scene()
scene.background = new THREE.Color(0xeeeeee)

// Kamera
const camera = new THREE.PerspectiveCamera(75, width3Dobject / height3Dobject, 0.1, 1000)
camera.position.z = 5

// Renderer
const renderer = new THREE.WebGLRenderer({ antialias: true })
renderer.setSize(width3Dobject, height3Dobject)
object3D.appendChild(renderer.domElement)

// Objek
const geometry = new THREE.BoxGeometry(4, 1, 5)
const material = [
    new THREE.MeshBasicMaterial({ color: 0xffff00 }), // kanan
    new THREE.MeshBasicMaterial({ color: 0xffff00 }), // kiri
    new THREE.MeshBasicMaterial({ color: 0x800080 }), // depan
    new THREE.MeshBasicMaterial({ color: 0x800080 }), // belakang
    new THREE.MeshBasicMaterial({ color: 0x00ffff }), // atas
    new THREE.MeshBasicMaterial({ color: 0x00ffff }), // bawah
]
const cube = new THREE.Mesh(geometry, material)
scene.add(cube)

// Animasi
function animate() {
    requestAnimationFrame(animate)
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            cube.rotation.x = data.pitch
            cube.rotation.y = data.yaw
            cube.rotation.z = data.roll

            tampilData(data)
        })
        .catch(error => console.error('Error:', error))
    renderer.render(scene, camera)
}
animate()

// Tampilkan data ke UI
function tampilData(data) {
    document.querySelector('#gyroX').innerText = Math.round(data.gyroscope.x)
    document.querySelector('#gyroY').innerText = Math.round(data.gyroscope.y)
    document.querySelector('#gyroZ').innerText = Math.round(data.gyroscope.z)

    document.querySelector('#accX').innerText = Math.round(data.acceleration.x)
    document.querySelector('#accY').innerText = Math.round(data.acceleration.y)
    document.querySelector('#accZ').innerText = Math.round(data.acceleration.z)

    document.querySelector('#roll').innerText = data.roll.toFixed(1)
    document.querySelector('#pitch').innerText = data.pitch.toFixed(1)
    document.querySelector('#yaw').innerText = data.yaw.toFixed(1)

    document.querySelector('#temp').innerText = Math.round(data.temperature)
}

// Resize the 3D object3D when the browser window changes size
window.addEventListener('resize', () => {
    width3Dobject = object3D.parentElement.clientWidth
    height3Dobject = object3D.parentElement.clientHeight
    camera.aspect = width3Dobject / height3Dobject
    camera.updateProjectionMatrix()
    renderer.setSize(width3Dobject, height3Dobject)
})