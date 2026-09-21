// Independent unit-action graph for offline route-efficiency diagnosis.
// Does not link to or run any planner, task matcher, or participant code.
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
int main(int argc,char** argv) {
    try {
        if(argc!=3)throw std::runtime_error("usage: oriented_distances map output.bin");
        std::ifstream input(argv[1]);if(!input)throw std::runtime_error("cannot open map");
        std::string line,key;int rows=0,cols=0;
        std::getline(input,line);std::getline(input,line);std::istringstream(line)>>key>>rows;
        std::getline(input,line);std::istringstream(line)>>key>>cols;std::getline(input,line);
        if(rows<=0 || cols<=0)throw std::runtime_error("invalid map dimensions");
        std::vector<int> grid(rows*cols,-1);std::vector<uint32_t> cells;
        for(int y=0;y<rows;++y) {
            if(!std::getline(input,line) || int(line.size())<cols)throw std::runtime_error("truncated map");
            for(int x=0;x<cols;++x)if(line[x]!='@' && line[x]!='T') {
                grid[y*cols+x]=int(cells.size());cells.push_back(uint32_t(y*cols+x));
            }
        }
        const int n=int(cells.size())*4;
        if(n==0 || n>16384)throw std::runtime_error("offline small-map distance limit exceeded");
        std::vector<std::array<int,3>> edges(n);
        const int dx[4]={1,0,-1,0},dy[4]={0,1,0,-1};
        for(int cell=0;cell<int(cells.size());++cell)for(int dir=0;dir<4;++dir) {
            const int x=int(cells[cell])%cols+dx[dir],y=int(cells[cell])/cols+dy[dir];
            const int neighbor=x>=0 && x<cols && y>=0 && y<rows?grid[y*cols+x]:-1;
            edges[cell*4+dir]={cell*4+(dir+1)%4,cell*4+(dir+3)%4,neighbor<0?-1:neighbor*4+dir};
        }
        std::ofstream output(argv[2],std::ios::binary);if(!output)throw std::runtime_error("cannot open output");
        const uint32_t header[5]={0x4f524431u,uint32_t(rows),uint32_t(cols),uint32_t(cells.size()),uint32_t(n)};
        output.write(reinterpret_cast<const char*>(header),sizeof(header));
        output.write(reinterpret_cast<const char*>(cells.data()),cells.size()*sizeof(uint32_t));
        std::vector<uint16_t> distance(n);std::vector<int> queue(n);
        for(int source=0;source<n;++source) {
            std::fill(distance.begin(),distance.end(),uint16_t(65535));distance[source]=0;
            int head=0,tail=1;queue[0]=source;
            while(head<tail) {
                const int v=queue[head++];
                for(int next:edges[v])if(next>=0 && distance[next]==65535) {
                    distance[next]=uint16_t(distance[v]+1);queue[tail++]=next;
                }
            }
            output.write(reinterpret_cast<const char*>(distance.data()),n*sizeof(uint16_t));
        }
        if(!output)throw std::runtime_error("failed distance output");
        std::cout<<cells.size()<<" cells, "<<n<<" oriented states\n";
    } catch(const std::exception& error) {std::cerr<<error.what()<<"\n";return 1;}
}
