Place plugin resources in this folder. They will be mounted at /.

Additional mount points:

- /X-Plane – Maps to the standard X-Plane root directory.
- /X-Plane/Resources/plugins – VFS restricted.
- /Aircraft – Points to the currently loaded aircraft's folder.
- /Aircraft/plugins – VFS restricted.

VFS restricted folders are redirected to ../vfs_lock. These paths are writable but contain no useful data.

This restriction prevents WASM modules from altering plugin configurations or affecting security policies.

VFS mount points are defined in ../config.json
