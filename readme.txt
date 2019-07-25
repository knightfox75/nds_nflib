[NOTA IMPORTANTE]
Dado que hace MUCHO tiempo que no actualizo la librería, esta no funciona «tal cual» en las últimas versiones de libNDS. Un recompilado de la librería usando la última versión de libNDS soluciona el problema. En el propio archivo comprimido de la librería hay un directorio para poder realizar esta recompilación.

NightFox’s Lib es una librería escrita en C que hemos desarrollado para facilitar la programación para DS. Basada en las Libnds de devkitArm, nuestra librería contiene funciones que hacen mucho más fácil el uso de fondos, sprites, mapas de colisiones, etc.

Las características básicas de la librería son:

- Soporte para fondos de tiles de 256 colores.
Puedes cargarlos desde FAT o NitroFS, pudiendo cargar hasta 32 fondos en RAM. El tamaño de los fondos debe ser en múltiplos de 256 pixeles. El sistema de fondos soporta paletas extendidas, pudiendo cargar hasta 16 paletas de 256 colores por fondo. La configuración de VRAM permite el uso de 96kb para tiles y 32kb para mapas por pantalla. El sistema también tiene soporte para mapas infinitos, usando la técnica «bank swap» manteniendo en RAM todo el mapa y copiando a la VRAM solo la parte necesaria. La única limitación es el tamaño del tileset, que no puede ser superior a 96kb entre las 4 capas. Este sistema también permite el scroll de los fondos, gestionando automáticamente que parte del mapa se carga en VRAM. También se incluyen funciones para mostrar u ocultar fondos, descargarlos de la RAM, cambiar tiles del mapa, modificar la paleta, uso de paletas extendidas, etc.
- Soporte para Sprites de 256 colores.
Se pueden cargar en RAM hasta 256 gráficos para sprites y 64 paletas. El sistema de sprites permite la creación de sprites, ocultarlos, rotarlos, cambiarles el tamaño, voltearlos, etc. Puedes colocar hasta 128 sprites por pantalla. El sistema también soporta sprites animados sin límite de frames, pudiendo escoger si los frames se mantienen en VRAM o solo se copia el frame mostrado. El sistema también soporta la modificación de paletas de los sprites para crear efectos.
- Motor de texto.
Basado en el sistema de fondos tileados, tiene soporte para fuentes de 8×8 y de 8×16 pixeles. Soporta fuentes de 256 colores (puedes usar fuentes ya coloreadas), definir tus propios colores, escoger la orientación del texto (normal, rotado a derecha, rotado a izquierda), realizar un scroll del texto, etc. Se debe cargar una fuente por cada capa de texto a usar (hasta 4 por pantalla).
- Sonido.
Soporte básico para la carga de archivos RAW desde FAT o NitroFS. Usa el engine de Libnds para reproducirlos. Puedes cargar hasta 32 sonidos en RAM.
- Mapas de colisión.
Muchos juegos requieren el uso de mapas de colisiones. Nuestra librería ofrece soporte de mapas de colisiones, editables desde código, pudiendo trabajar hasta con 32 mapas a la vez en RAM.
- Fondos en 8 y 16 bits.
Puedes usar fondos e imágenes de 8 y 16 bits si necesitas más calidad en pantalla. Se incluyen funciones para la carga y dibujado de estos fondos en pantalla. Puedes cargar hasta 16 imágenes en formato RAW de 8 o 16 bits. Este sistema también tiene soporte para backbuffer.
- Media.
La librería dispone funciones para la carga y visualizaciones de archivos BITMAP. Puedes cargar archivos de 8, 16 o 24 bits y mostrarlos en la pantalla (en modo 16 bits). La librería carga y convierte el archivo BMP por ti.
- 3d Sprites.
Funciones únicas para el uso del motor 3D de la consola, con el que se pueden crear hasta 256 Sprites en pantalla de manera simultánea con un tamaño máximo de 1024×1024, con posibilidad de rotación, escalado y hasta 62 unidades de transparencia diferentes.
- WIFI.
Soporte muy básico para la comunicación por wifi. Solo se incluyen unas funciones de prueba para la comunicación usando el protocolo UDP.
- Ejemplos.
La librería incluye más de 30 ejemplos listos para compilar, en los que se muestran prácticamente todas las funciones de la librería.
- Documentación.
Completa documentación en PDF, con una descripción completa de todas las funciones.
