# Nom de l'exécutable
TARGET = raytracer

# Compilateur et flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Répertoires
SRC_DIR_IMAGE = Image
SRC_DIR_VEC = Vecteurs

# Fichiers source
SRCS = $(SRC_DIR_IMAGE)/Raytracer.cpp \
       $(SRC_DIR_IMAGE)/Image.cpp \
       $(SRC_DIR_VEC)/Vector.cpp

# Fichiers objets (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Règle par défaut
all: $(TARGET)

# Édition de liens
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers temporaires
clean:
	rm -f $(OBJS) $(TARGET)

run:
	./raytracer

# Nettoyage + fichiers générés (si tu veux aussi supprimer rouge.ppm par exemple)
distclean: clean
	rm -f Image/*.ppm
