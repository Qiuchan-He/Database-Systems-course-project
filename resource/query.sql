SELECT id, pkg_name, size / 1024 / 1024, "MiB" FROM packages WHERE size > 1024 * 1024 * 1024;
