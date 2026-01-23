<?php
$upload_dir = "uploads/";
$info_file = $upload_dir . "info.json";
$upload_info = [];

if (file_exists($info_file)) {
    $upload_info = json_decode(file_get_contents($info_file), true) ?: [];
}

$upload_info = array_reverse($upload_info);

$page_title = "Image Gallery";
$current_page = "gallery";

include 'templates/header.html';
include 'templates/gallery_content.html';
include 'templates/footer.html';
include 'templates/gallery_modal.html';
?>