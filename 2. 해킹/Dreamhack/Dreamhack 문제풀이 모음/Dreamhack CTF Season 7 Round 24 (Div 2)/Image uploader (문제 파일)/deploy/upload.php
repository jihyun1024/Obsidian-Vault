<?php
session_start();

$upload_dir = "uploads/";
if (!file_exists($upload_dir)) {
    mkdir($upload_dir, 0755, true);
}

$info_file = $upload_dir . "info.json";
$upload_info = [];

if (file_exists($info_file)) {
    $upload_info = json_decode(file_get_contents($info_file), true) ?: [];
}

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_FILES['file'])) {
    $file = $_FILES['file'];
    $title = $_POST['title'] ?? '';
    $description = $_POST['description'] ?? '';
    
    if ($file['error'] !== UPLOAD_ERR_OK) {
        die("<script>alert('Error uploading file.'); history.back();</script>");
    }
    
    if ($file['size'] > 5 * 1024 * 1024) {
        die("<script>alert('File size too large.'); history.back();</script>");
    }
    
    $filename = basename($file['name']);
    $file_extension = strtolower(pathinfo($filename, PATHINFO_EXTENSION));
    
    $allowed_extensions = ['jpg', 'jpeg', 'png', 'gif'];
    
    $check_extension = $file_extension;
    
    $finfo = finfo_open(FILEINFO_MIME_TYPE);
    $mime_type = finfo_file($finfo, $file['tmp_name']);
    finfo_close($finfo);
    
    $allowed_mimes = ['image/jpeg', 'image/png', 'image/gif', 'image/jpg'];
    
    if (!in_array($mime_type, $allowed_mimes) && !in_array($check_extension, $allowed_extensions)) {
        die("<script>alert('Only images allowed.'); history.back();</script>");
    }
    
    $new_filename = date('YmdHis') . '_' . mt_rand(1000, 9999) . '_' . $filename;
    $target_path = $upload_dir . $new_filename;
    
    if (move_uploaded_file($file['tmp_name'], $target_path)) {
        $upload_info[] = [
            'filename' => $new_filename,
            'original_name' => $filename,
            'title' => htmlspecialchars($title),
            'description' => htmlspecialchars($description),
            'upload_time' => date('Y-m-d H:i:s'),
            'size' => $file['size'],
            'mime_type' => $mime_type
        ];
        
        file_put_contents($info_file, json_encode($upload_info, JSON_PRETTY_PRINT));
        
        echo "<script>alert('File uploaded successfully!'); location.href='gallery.php';</script>";
    } else {
        echo "<script>alert('Failed to upload file.'); history.back();</script>";
    }
} else {
    echo "<script>alert('Invalid access.'); location.href='index.php';</script>";
}
?>