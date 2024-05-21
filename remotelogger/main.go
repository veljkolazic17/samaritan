package main

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"

	"github.com/go-redis/redis/v8"
)

func main() {
	// Connect to Redis
	rdb := redis.NewClient(&redis.Options{
		Addr:     "localhost:6379", // Redis server address
		Password: "",               // No password set
		DB:       0,                // Use default DB
	})

	// Ping Redis to test connection
	pong, err := rdb.Ping(rdb.Context()).Result()
	if err != nil {
		log.Fatalf("Error connecting to Redis: %v", err)
	}
	log.Printf("Connected to Redis, got pong: %s", pong)

	// Define an HTTP handler
	http.HandleFunc("/{channel}", func(w http.ResponseWriter, r *http.Request) {
		// Fetch data from Redis or perform any other desired action

		//channel := r.PathValue("channel")

		tmpl := template.Must(template.ParseFiles("index.html"))
		films := map[string][]Film{
			"Films": {
				{Title: "The Godfather", Director: "Francis Ford Coppola"},
				{Title: "Blade Runner", Director: "Ridley Scott"},
				{Title: "The Thing", Director: "John Carpenter"},
			},
		}
		tmpl.Execute(w, films)

		data, err := rdb.Get(rdb.Context(), "example-key").Result()
		if err != nil {
			http.Error(w, "Error fetching data from Redis", http.StatusInternalServerError)
			return
		}
		fmt.Fprintf(w, "Data from Redis: %s", data)
	})

	// Start the HTTP server
	server := &http.Server{
		Addr: ":8080", // Listen on port 8080
	}

	go func() {
		if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("Error starting HTTP server: %v", err)
		}
	}()

	// Graceful shutdown handling
	stop := make(chan os.Signal, 1)
	signal.Notify(stop, syscall.SIGINT, syscall.SIGTERM)
	<-stop

	log.Println("Shutting down...")

	if err := server.Shutdown(rdb.Context()); err != nil {
		log.Fatalf("Error shutting down server: %v", err)
	}

	// Close Redis connection
	if err := rdb.Close(); err != nil {
		log.Fatalf("Error closing Redis connection: %v", err)
	}

	log.Println("Server stopped.")
}
