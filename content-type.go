package main

import (
	"fmt"
	"log"
	"net/http"
	"net/url"
	"os"
)

func content(testUrl string) (string, error) {

	// We read in 512 bytes 
	buf := make([]byte, 512)

	u, err := url.ParseRequestURI(testUrl)
	if err != nil {
		return "", err
	}
	r, err := http.Get(u.String())
	if err != nil {
		return "", err
	}
	defer r.Body.Close()
	n, err := r.Body.Read(buf)
	if err != nil {
		return "", err
	}
	mime := http.DetectContentType(buf[:n])
	if mime == "application/octet-stream" {
		mime = r.Header.Get("Content-type")
		return mime, nil
	}
	return mime, nil
	
}

func main() {
	if (len(os.Args) != 2) {
		log.SetFlags(0)
		log.Fatalf("Usage: %s <URL>\n", os.Args[0])
	}
	ct, err := content(os.Args[1])
	if err != nil {
		log.Fatalf("Error fetching content-type for URL: %s\n", err)
	}
	fmt.Println(ct)
}
